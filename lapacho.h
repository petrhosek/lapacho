#ifndef LAPACHO_H_
#define LAPACHO_H_

#include <stddef.h>
#include <limits.h>

/* Python 3 support */
#if PY_MAJOR_VERSION >= 3
  #define PyInt_AsLong PyLong_AsLong
  #define PyInt_Check PyLong_Check
  #define PyInt_FromLong PyLong_FromLong
  #define PyString_AS_STRING PyBytes_AS_STRING
  #define PyString_AsString PyBytes_AsString
  #define PyString_AsStringAndSize PyBytes_AsStringAndSize
  #define PyString_Check PyBytes_Check
  #define PyString_FromString PyBytes_FromString
  #define PyString_FromStringAndSize PyBytes_FromStringAndSize
  #define PyString_Size PyBytes_Size
#endif

#if PY_MAJOR_VERSION == 2
  #define to_path(x) to_bytes(x)
  #define to_encoding(x) to_bytes(x)
#else
  #define to_path(x) to_unicode(x, Py_FileSystemDefaultEncoding, "strict")
  #define to_encoding(x) PyUnicode_DecodeASCII(x, strlen(x), "strict")
#endif

struct matrix {
    size_t         cols;
    size_t         rows;
    size_t         colstep;
    size_t         rowstep;
    size_t         references;
    double         data[];
};

struct matrix_view {
    int            cols;
    int            rows;
    size_t         colstep;
    size_t         rowstep;
    struct matrix *parent;
    double        *data;
};

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#define BITS_PER_LONG __WORDSIZE
#define BITS_PER_BYTE 8
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define __ALIGN_MASK(x, mask) (((x) + (mask))&~(mask))
#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a) - 1)

#ifndef max
#define max(x, y) ({ \
    typeof(x) _max1 = (x); \
    typeof(y) _max2 = (y); \
    (void) (&_max1 == &_max2); \
    _max1 > _max2 ? _max1 : _max2; })
#endif

#ifndef min
#define min(x, y) ({ \
    typeof(x) _min1 = (x); \
    typeof(y) _min2 = (y); \
    (void)(&_min1 == &_min2); \
    _min1 < _min2 ? _min1 : _min2; })
#endif

static inline void set_bit(int nr, long *addr) {
    addr[nr / BITS_PER_LONG] |= 1UL << (nr % BITS_PER_LONG);
}

static inline void clear_bit(int nr, long *addr) {
    addr[nr / BITS_PER_LONG] &= ~(1UL << (nr % BITS_PER_LONG));
}

static inline int test_bit(unsigned int nr, const long *addr) {
    return ((1UL << (nr % BITS_PER_LONG)) &
        (((unsigned long *)addr)[nr / BITS_PER_LONG])) != 0;
}

extern int hungarian(double **costs, size_t m, size_t n, long *rows, long *cols);

#endif
