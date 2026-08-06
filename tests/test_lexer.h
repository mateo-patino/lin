#ifndef TEST_LEXER_H
#define TEST_LEXER_H

#include "test_helpers.h"


/*
* Struct for arranging a string expression and the expected dimensions and data
* into one container. 
*/
typedef struct {
    const char *str;
    uint nrow;
    uint ncol;
    const scalar_t *expected_data;
} matrix_test_case_t;


/* Helper to the ASSERT_EQ_MATDATA macro */
bool equal_matrix_data(const scalar_t *actual, const scalar_t *expected, size_t sz);

/*
* Runs all tests in the lexer suite .
*
* Returns the number of successful tests and writes the number of crashes
* or signal terminations to `crash` and the total number of tests to `total`.
*/
uint run_lexer_tests(uint *total, uint *crashes);


#endif
