#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <stdbool.h>
#include <stdlib.h>

#include "types/matrix.h"
#include "types/token.h"
#include "parser/parser.h"
#include "test_helpers.h"

/* Assert `expr` is true */
void assert_true_failed(const char *expr, const char *file, int line, const char *func);
#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            assert_true_failed(#x, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)

/* Assert scalar values `actual` and `expected` are equal */
void assert_eq_scalar_failed(scalar_t actual, scalar_t expected,
                             const char *actual_expr, const char *expected_expr,
                             const char *file, int line, const char *func);
#define ASSERT_EQ_SCALAR(actual, expected) \
    do { \
        scalar_t actual_value = (actual); \
        scalar_t expected_value = (expected); \
        if (actual_value != expected_value) { \
            assert_eq_scalar_failed(actual_value, expected_value, #actual, #expected, \
                                    __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)


/* Assert the arrays `actual` and `expected` are identical. */

void assert_eq_matdata_failed(const scalar_t *actual, const scalar_t *expected, size_t sz, const char *file, 
                            int line, const char *func);
bool equal_matrix_data(const scalar_t *actual, const scalar_t *expected, size_t sz);
#define ASSERT_EQ_MATDATA(actual, expected, sz) \
    do { \
        const scalar_t *_actual = actual; \
        const scalar_t *_expected = expected; \
        size_t _sz = sz; \
        if (!equal_matrix_data(_actual, _expected, _sz)) { \
            assert_eq_matdata_failed(_actual, _expected, _sz, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)


/* Assert that `node` points to node_t containing an operator of type `op`. */
bool assert_operator_node(const node_t *node, operator_type op);
void assert_operator_node_failed(operator_type op, const char *file, int line, const char *func);
#define ASSERT_OPERATOR_NODE(node, op) \
    do { \
        const node_t *_node = (const node_t *)(node); \
        operator_type _op = (op); \
        if (!assert_operator_node(_node, _op)) { \
            assert_operator_node_failed(_node, _op, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0) 


#endif
