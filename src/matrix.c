#include "matrix.h"

#include <stdlib.h>


void free_matrix(matrix_t *mat) {
    if (!mat) {
        return;
    }
    free(mat->data);
    free(mat);
}


matrix_t *init_matrix(scalar_t *data, unsigned int nrow, unsigned int ncol) {
    matrix_t *mat = malloc(sizeof(matrix_t));
    if (!mat) {
        return NULL;
    }
    mat->data = data;
    mat->nrow = nrow;
    mat->ncol = ncol;

    return mat;
}
