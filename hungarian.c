#include "lapacho.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

double find_smallest_uncovered(double **costs, int *row_covered,
        int *col_covered, size_t size);
int find_uncovered_zero(double **costs, int *row_covered,
        int *col_covered, size_t size, int *row, int *col);
int find_star_in_row(int **marked, size_t size, int row) __attribute__((const));
int find_star_in_col(int **marked, size_t size, int col) __attribute__((const));
int find_prime_in_row(int **marked, size_t size, int row) __attribute__((const));
void convert_path(int **marked, int **path, size_t count);
void clear_covers(int *row_covered, int *col_covered, size_t size);
void erase_primes(int **marked, size_t size);

enum {
    NONE  = 0,
    STAR  = 1,
    PRIME = 2,
};

/* Pad a m*n matrix to square size*size matrix. */
double **pad_matrix(double **matrix, size_t rows, size_t cols, size_t size)
{
    size_t i, j;
    double **new_matrix, *strides;
    size_t matrix_bytes, row_bytes, col_bytes;

    row_bytes = size * sizeof(double *);
    col_bytes = size * sizeof(double);
    matrix_bytes = row_bytes + size * col_bytes;
    new_matrix = malloc(matrix_bytes);
    assert(new_matrix != NULL);
    memset(new_matrix, 0, matrix_bytes);

    strides = (double *)(new_matrix + size);
    for (i = 0; i < size; i++)
        new_matrix[i] = strides + i * size;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++)
            new_matrix[i][j] = matrix[i][j];
    }

    return new_matrix;
}

/* Create an n*n matrix, populating it with the specific value. */
int **make_matrix(size_t rows, size_t cols)
{
    size_t i;
    int **matrix, *strides;
    size_t matrix_bytes, row_bytes, col_bytes;

    row_bytes = rows * sizeof(int *);
    col_bytes = cols * sizeof(int);
    matrix_bytes = row_bytes + rows * col_bytes;
    matrix = malloc(matrix_bytes);
    assert(matrix != NULL);
    memset(matrix, 0, matrix_bytes);

    strides = (int *)(matrix + rows);
    for (i = 0; i < rows; i++)
        matrix[i] = strides + i * cols;

    return matrix;
}

int hungarian(double **costs, size_t m, size_t n, long *rows, long *cols) {
    size_t size = MAX(m, n);
    int *row_covered, *col_covered;
    int **marked;
    int *row_prev, *col_prev;
    int count;
    int i, j;
    double min;

    row_covered = (int *)calloc(size, sizeof(int));
    col_covered = (int *)calloc(size, sizeof(int));

    row_prev = (int *)calloc(size * 2, sizeof(int));
    col_prev = (int *)calloc(size * 2, sizeof(int));

    costs = pad_matrix(costs, m, n, size);
    marked = make_matrix(size, size);

step1:
    /*
     * For each row of the matrix, find the smallest element and
     * subtract it from every element in its row. Go to Step 2.
     */
    for (i = 0; i < size; i++) {
        min = costs[i][0];

        /* find the minimum value for this row */
        for (j = 1; j < size; j++) {
            if (min > costs[i][j])
                min = costs[i][j];
        }

        /* substract minimum from every element in the row */
        for(j = 0; j < size; j++) {
            costs[i][j] -= min;
        }
    }

step2:
    /*
     * Find a zero (Z) in the resulting matrix. If there is no starred
     * zero in its row or column, star Z. Repeat for each element in the
     * matrix. Go to Step 3.
     */
    for (i = 0; i < size; i++) {
        if (row_covered[i])
            continue;

        for (j = 0; j < size; j++) {
            if (col_covered[j])
                continue;

            if (costs[i][j] == 0) {
                marked[i][j] = STAR;
                row_covered[i] = 1;
                col_covered[j] = 1;
                break;
            }
        }
    }
    clear_covers(row_covered, col_covered, size);

step3:
    /*
     * Cover each column containing a starred zero. If K columns are
     * covered, the starred zeros describe a complete set of unique
     * assignments. In this case, go to done, otherwise, go to Step 4.
     */
    count = 0;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (marked[i][j] == STAR) {
                col_covered[j] = 1;
                count++;
            }
        }
    }
    if (count >= size)
        goto step7;

step4:
    /*
     * Find a noncovered zero and prime it. If there is no starred zero
     * in the row containing this primed zero, Go to Step 5. Otherwise,
     * cover this row and uncover the column containing the starred
     * zero. Continue in this manner until there are no uncovered zeros
     * left. Save the smallest uncovered value and Go to Step 6.
     */
    while (1) {
        int row, col, star_col;

        if (!find_uncovered_zero(costs, row_covered, col_covered, size, &row, &col))
            goto step6;

        marked[row][col] = PRIME;
        star_col = find_star_in_row(marked, size, row);
        if (star_col >= 0) {
            row_covered[row] = 1;
            col_covered[star_col] = 0;
        } else {
            row_prev[0] = row;
            col_prev[0] = col;
            goto step5;
        }
    }

step5:
    /*
     * Construct a series of alternating primed and starred zeros as
     * follows. Let Z0 represent the uncovered primed zero found in Step 4.
     * Let Z1 denote the starred zero in the column of Z0 (if any).
     * Let Z2 denote the primed zero in the row of Z1 (there will always
     * be one). Continue until the series terminates at a primed zero
     * that has no starred zero in its column. Unstar each starred zero
     * of the series, star each primed zero of the series, erase all
     * primes and uncover every line in the matrix. Return to Step 3.
     */
    count = 0;
    while (1) {
        int row, col;

        row = find_star_in_col(marked, size, col_prev[count]);
        if (row >= 0) {
            count++;
            row_prev[count] = row;
            col_prev[count] = col_prev[count - 1];
        } else break;

        col = find_prime_in_row(marked, size, row_prev[count]);
        count++;
        row_prev[count] = row_prev[count - 1];
        col_prev[count] = col;
    }

    for (i = 0; i <= count; i++) {
        int row = row_prev[i], col = col_prev[i];
        if (marked[row][col] == STAR)
            marked[row][col] = NONE;
        else if (marked[row][col] == PRIME)
            marked[row][col] = STAR;
    }
    clear_covers(row_covered, col_covered, size);
    erase_primes(marked, size);

    goto step3;

step6:
    /*
     * Add the value found in Step 4 to every element of each covered
     * row, and subtract it from every element of each uncovered column.
     * Return to Step 4 without altering any stars, primes, or covered
     * lines.
     */
    min = find_smallest_uncovered(costs, row_covered, col_covered, size);
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (row_covered[i])
                costs[i][j] += min;
            if (!col_covered[j])
                costs[i][j] -= min;
        }
    }
    goto step4;

step7:
    count = 0;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if (marked[i][j] == STAR) {
                rows[count] = i;
                cols[count] = j;
                count++;
            }
        }
    }

out:
    free(row_covered);
    free(col_covered);
    free(row_prev);
    free(col_prev);
    free(costs);
    free(marked);
    return count;
}

/* Find the smallest uncovered value in the matrix. */
double find_smallest_uncovered(double **costs, int *row_covered,
        int *col_covered, size_t size)
{
    int i, j;
    double min = DBL_MAX;

    for (i = 0; i < size; i++) {
        if (row_covered[i])
            continue;

        for (j = 0; j < size; j++) {
            if (col_covered[j])
                continue;

            if (min > costs[i][j])
                min = costs[i][j];
        }
    }

    return min;
}

/* Find the first uncovered element with value 0 */
int find_uncovered_zero(double **costs, int *row_covered,
        int *col_covered, size_t size, int *row, int *col)
{
    int i, j;

    for (i = 0; i < size; i++) {
        if (row_covered[i])
            continue;

        for (j = 0; j < size; j++) {
            if (col_covered[j])
                continue;

            if (costs[i][j] == 0.0) {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }

    return 0;
}

/*
 * Find the first starred element in the specified row. Returns
 * the column index, or -1 if no starred element was found.
 */
int find_star_in_row(int **marked, size_t size, int row)
{
    int j;

    for (j = 0; j < size; j++) {
        if (marked[row][j] == STAR)
            return j;
    }
    return -1;
}

/*
 * Find the first starred element in the specified row. Returns
 * the row index, or -1 if no starred element was found.
 */
int find_star_in_col(int **marked, size_t size, int col)
{
    int i;

    for (i = 0; i < size; i++) {
        if (marked[i][col] == STAR)
            return i;
    }
    return -1;
}

/*
 * Find the first prime element in the specified row. Returns
 * the column index, or -1 if no starred element was found.
 */
int find_prime_in_row(int **marked, size_t size, int row)
{
    int j;

    for (j = 0; j < size; j++) {
        if (marked[row][j] == PRIME)
            return j;
    }
    return -1;
}

void convert_path(int **marked, int **path, size_t count)
{
    int i;

    for (i = 0; i < count + 1; i++) {
        if (marked[path[i][0]][path[i][1]] == 1)
            marked[path[i][0]][path[i][1]] = 0;
        else
            marked[path[i][0]][path[i][1]] = 1;
    }
}

/* Clear all covered matrix cells */
void clear_covers(int *row_covered, int *col_covered, size_t size)
{
    int i;

    for (i = 0; i < size; i++) {
        row_covered[i] = 0;
        col_covered[i] = 0;
    }
}

/* Erase all prime markings */
void erase_primes(int **marked, size_t size)
{
    int i, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (marked[i][j] == PRIME)
                marked[i][j] = 0;
        }
    }
}
