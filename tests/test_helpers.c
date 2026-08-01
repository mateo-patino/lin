#include "test_helpers.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>


void perr(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, fmt, args);
    va_end(args);
}


void print_success(test_case_t *test_case) {
    fprintf(stdout, BOLD "%s " ANSI_RESET ANSI_GREEN "PASS\n" ANSI_RESET, test_case->test_name);
}


int run_in_sandbox(const test_case_t *test_case, int *signum) {

    pid_t pid;
    
    if ((pid = fork()) < 0) {
        fprintf(stderr, "Error: fork() failed. Could not test %s\n", test_case->test_name);
        return -1;
    }
    /* Child process */
    else if (pid == 0) {
        if (test_case->func()) {
            _exit(EXIT_SUCCESS);
        }
        _exit(EXIT_FAILURE);
    }

    /* Parent process */
    int status;
    errno = 0;
    pid_t term_pid = waitpid(pid, &status, 0);
    while (term_pid == -1 && errno == EINTR) {
        term_pid = waitpid(pid, &status, 0);
    }

    if (term_pid == -1) {
        fprintf(stderr, "Error: waitpid() failed. Could not read exit status of %s\n", 
                test_case->test_name);
        return -1;
    }

    /* Check for normal program termination */
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == EXIT_SUCCESS) {
            return 0;
        }
        return 1;
    }
    /* Check for termination by signal */
    else if (WIFSIGNALED(status)) {
        if (signum) { *signum =  WTERMSIG(status); }
        return 2;
    }

    /* We should never get here, but if we do, test must've failed regardless */
    return 1;
}

