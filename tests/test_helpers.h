#ifndef HELPERS_H
#define HELPERS_H

#include "types/matrix.h"

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
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))


/* 
* Calls fprintf(stderr, fmt, ...). Used to print error messages when a test fails.
*/
void perr(const char *fmt, ...);


/*
* Returns a pointer to a string representing the common name associated
* to a POSIX signal num `signum` (e.g. 11 -> SIGSEGV)
*/
const char *signum_to_str(int signum);

/*
* Prints test_case->name CRASHED (signal signum) to stderr
*/
void print_crash(const test_case_t *test_case, int signum);


/*
* Prints test_case->name PASS\n to stdout.
*/
void print_success(const test_case_t *test_case);


/*
* Returns true if abs(`a` - `b`) < `tol` (i.e. if a and b are close).
*/
bool is_close(scalar_t a, scalar_t b, scalar_t abs_tol, scalar_t rel_tol);


/*
* Forks and calls `func`. 
*
* If `func` fails, 1 will be returned. If the child process  is terminated by a
* POSIX signal, 2 is returned and the signal number is written to `signum`. 
* If `func` succeeds, 0 is returned.
* 
* If a POSIX system call fails, -1 is returned (hence, use a signed int to store 
* the result NOT uint).
*/
int run_in_sandbox(const test_case_t *test_case, int *signum);




#endif
