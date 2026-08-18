#ifndef TEST_LEXER_H
#define TEST_LEXER_H

#include "test_helpers.h"


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
