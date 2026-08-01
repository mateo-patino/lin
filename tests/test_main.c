#include "test_lexer.h"
#include <stdio.h>
#include <stdlib.h>


#define LINE "------------"

static void print_overall_summary(uint total, uint pass, uint crash) {
    fprintf(stdout, BOLD LINE "SUMMARY" LINE ANSI_RESET "\n");
    fprintf(stdout, BOLD "Total tests: %i" ANSI_RESET "\n", total);
    fprintf(stdout, BOLD "Successful: %i" ANSI_RESET "\n", pass);
    fprintf(stdout, BOLD "Failed: %i (%i crash)" ANSI_RESET "\n", total-pass, crash);
    fprintf(stdout, BOLD ANSI_CYAN "Overall success %.0f%%" ANSI_RESET "\n", 100 * (float)pass / total);
}


static void print_test_summary(const char *kind, uint total, uint pass, uint crash) {
    fprintf(stdout, BOLD "%s Results: %i total, %i pass, %i fail (%i crash)\n" ANSI_RESET,
            kind, total, pass, total-pass, crash); 
}


int main(void) {

    /* Global result counters */
    uint total_tests = 0;
    uint total_pass = 0;
    uint total_crash = 0;

    /* Lexer tests */
    fputc('\n', stdout);
    fprintf(stdout, BOLD LINE "LEXER TESTS" LINE ANSI_RESET "\n"); 
    uint total_lexer = 0;
    uint crash_lexer = 0;
    uint pass_lexer = run_lexer_tests(&total_lexer, &crash_lexer);
    print_test_summary("Lexer", total_lexer, pass_lexer, crash_lexer);
    
    total_tests += total_lexer;
    total_pass += pass_lexer;
    total_crash += crash_lexer;
    
    /* Overall summary */
    print_overall_summary(total_tests, total_pass, total_crash);
    
    return EXIT_SUCCESS;
}
