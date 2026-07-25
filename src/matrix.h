#ifndef MATRIX_H
#define MATRIX_H

/* Scalar type */
typedef double scalar_t;

/* Matrix interface... */
typedef struct {
    int nrows;
    int ncols;
    scalar_t *data;
} matrix_t;


#endif
