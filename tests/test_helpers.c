#include "test_helpers.h"
#include "types/matrix.h"
#include "types/token.h"

#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h> /* Requires POSIX */
#include <sys/wait.h> /* Requires POSIX */
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


void perr(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}


const char *operator_to_str(operator_type op) {
    switch (op) {
        case ADD:
            return "ADD";
        case SUB:
            return "SUB";
        case MUL:
            return "MUL";
        case DIV:
            return "DIV";
        case DET:
            return "DET";
        case INV:
            return "INV";
        case RREF:
            return "RREF";
        case NUM_OP:
            return "NUM_OP";
        default:
            return "'Unknown operator'";
    }
}


const char *signum_to_str(int signum) {
    switch (signum) {
        case -1:
            return "";
        case SIGHUP:
            return "SIGHUP";
        case SIGINT:
            return "SIGINT";
        case SIGQUIT:
            return "SIGQUIT";
        case SIGILL:
            return "SIGILL";
        case SIGTRAP:
            return "SIGTRAP";
        case SIGABRT:
            return "SIGABRT";
        case SIGBUS:
            return "SIGBUS";
        case SIGFPE:
            return "SIGFPE";
        case SIGKILL:
            return "SIGKILL";
        case SIGUSR1:
            return "SIGUSR1";
        case SIGSEGV:
            return "SIGSEGV";
        case SIGUSR2:
            return "SIGUSR2";
        case SIGPIPE:
            return "SIGPIPE";
        case SIGALRM:
            return "SIGALRM";
        case SIGTERM:
            return "SIGTERM";
        case SIGCHLD:
            return "SIGCHLD";
        case SIGCONT:
            return "SIGCONT";
        case SIGSTOP:
            return "SIGSTOP";
        case SIGTSTP:
            return "SIGTSTP";
        case SIGTTIN:
return "SIGTTIN";
        case SIGTTOU:
            return "SIGTTOU";
        case SIGURG:
            return "SIGURG";
        case SIGXCPU:
            return "SIGXCPU";
        case SIGXFSZ:
            return "SIGXFSZ";
        case SIGVTALRM:
            return "SIGVTALRM";
        case SIGPROF:
            return "SIGPROF";
        case SIGWINCH:
            return "SIGWINCH";
        case SIGSYS:
            return "SIGSYS";
        default:
            return "UNKNOWN";
    }
}


void print_crash(const test_case_t *test_case, int signum) {
    fprintf(stderr, BOLD "%s" ANSI_RED " CRASHED (signal %i, %s)\n" ANSI_RESET,
            test_case->test_name, signum, signum_to_str(signum));
}


void print_success(const test_case_t *test_case) {
    fprintf(stderr, BOLD "%s " ANSI_RESET ANSI_GREEN "PASS\n" ANSI_RESET, test_case->test_name);
}


bool is_close(scalar_t a, scalar_t b, scalar_t abs_tol, scalar_t rel_tol) { 
    /* a == b distinguishes between +INF and -INF (i.e +INF != -INF returns false) */
    if ((isnan(a) && isnan(b)) || (isinf(a) && a == b)) {
        return true;
    }

    scalar_t diff = fabs(a - b);
    if (diff <= abs_tol) {
        return true;
    }
    return diff <= rel_tol * fmax(fabs(a), fabs(b));
}


void print_matrix_data(FILE *stream, const scalar_t *data, size_t sz, bool add_newline) {
    if (!stream || !data || !sz) {
        return;
    }
    int digits = 2;
    for (size_t i = 0; i < sz; i++) {
        fprintf(stream, PRISCALAR, digits, data[i]);
        if (i < sz-1) {
            fputc(' ', stream);
        }
    }
    if (add_newline) {
        fputc('\n', stream);
    }
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
