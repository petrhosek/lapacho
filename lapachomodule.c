#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "lapacho.h"

double **
read_matrix(PyObject *py_matrix, Py_ssize_t *m, Py_ssize_t *n) {
    Py_ssize_t i, j;
    Py_ssize_t total_rows, total_cols;
    size_t matrix_bytes, row_bytes, col_bytes;
    double **matrix, *strides;

    total_rows = PyList_Size(py_matrix);
    total_cols = 0;
    for (i = 0; i < total_rows; i++) {
        PyObject *py_row;

        py_row = PyList_GetItem(py_matrix, i);
        if (!PyList_Check(py_row)) {
            PyErr_SetObject(PyExc_TypeError, py_row);
            return NULL;
        }
        total_cols = max(total_cols, PyList_Size(py_row));
    }

    row_bytes = total_rows * sizeof(double *);
    col_bytes = total_cols * sizeof(double);
    matrix_bytes = row_bytes + total_rows * col_bytes;
    matrix = malloc(matrix_bytes);
    memset(matrix, 0, matrix_bytes);

    strides = (double *)(matrix + total_rows);
    for (i = 0; i < total_rows; i++) {
        PyObject *py_row;
        Py_ssize_t row_len;

        matrix[i] = strides + i * total_cols;

        py_row = PyList_GetItem(py_matrix, i);
        row_len = PyList_Size(py_row);
        for (j = 0; j < row_len; ++j) {
            PyObject *py_value;

            py_value = PyList_GetItem(py_row, j);
            matrix[i][j] = (double)PyFloat_AsDouble(py_value);
            if (PyErr_Occurred() != NULL) {
                PyErr_SetObject(PyExc_TypeError, py_value);
                return NULL;
            }
        }
    }

    *m = total_rows;
    *n = total_cols;

    return matrix;
}

PyObject *
lapacho_hungarian(PyObject *self, PyObject *args)
{
    PyObject *py_matrix;
    PyObject *py_result = NULL;
    Py_ssize_t m, n;

    long *rows = NULL, *cols = NULL;
    double **matrix = NULL;
    int index_count;
    int i;

    if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &py_matrix))
        return NULL;

    matrix = read_matrix(py_matrix, &m, &n);

    rows = (long *)calloc(m, sizeof(long));
    if (!rows) {
        PyErr_NoMemory();
        goto out;
    }
    cols = (long *)calloc(n, sizeof(long));
    if(!cols) {
        PyErr_NoMemory();
        goto out;
    }

    /* Run the matching algorithm */
    index_count = hungarian(matrix, m, n, rows, cols);
    py_result = PyList_New(index_count);
    if (!py_result)
        goto out;

    for (i = 0; i < index_count; ++i) {
        PyObject *tuple = Py_BuildValue("(ll)", rows[i], cols[i]);
        PyList_SetItem(py_result, i, tuple);
    }

out:
    free(matrix);
    free(rows);
    free(cols);
    return py_result;
}

PyMethodDef module_methods[] = {
    {"hungarian", lapacho_hungarian, METH_VARARGS,
     "Solves the linear assignment problem using the Hungarian algorithm."
     "The function takes a single argument - a square cost matrix - and"
     " returns a\ntuple of the form\n(row_assigns,col_assigns)."},
    {NULL}
};

PyObject*
moduleinit(PyObject* m)
{
    if (m == NULL)
        return NULL;

    return m;
}

#if PY_MAJOR_VERSION < 3
  PyMODINIT_FUNC
  init_lapacho(void)
  {
      PyObject* m;
      m = Py_InitModule3("_lapacho", module_methods,
                         "Python bindings for libgit2.");
      moduleinit(m);
  }
#else
  struct PyModuleDef moduledef = {
      PyModuleDef_HEAD_INIT,
      "_lapacho",                      /* m_name */
      "Python bindings for libgit2.",  /* m_doc */
      -1,                              /* m_size */
      module_methods,                  /* m_methods */
      NULL,                            /* m_reload */
      NULL,                            /* m_traverse */
      NULL,                            /* m_clear */
      NULL,                            /* m_free */
  };

  PyMODINIT_FUNC
  PyInit__lapacho(void)
  {
      PyObject* m;
      m = PyModule_Create(&moduledef);
      return moduleinit(m);
  }
#endif
