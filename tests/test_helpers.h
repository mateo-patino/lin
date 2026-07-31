#ifndef HELPERS_H
#define HELPERS_H

/* THIS FILE CONTAINS VARIOUS HELPERS USED BY THE TEST SUITE */

/* ANSI COLORS */
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_RESET "\x1b[0m"
#define BOLD "\x1b[1m"


/* Unsigned int */
typedef unsigned int uint;

/* Test case type */
typedef bool (*test_func_t)(void);
typedef struct {
    const char *test_name;
    test_func_t func;
} test_case_t;

/* For brace initializing a test_case_t */
#define TEST(x) {#x, x}

/* (Continuous) array length */
#define ARRAY_LEN(x) (sizeof(x) / sizeof((a)[0]))


/* 
* Calls fprintf(stderr, fmt, ...). Used to print error messages when a test fails.
*/
void perr(const char *fmt, ...);

#endif
