#include "test_lexer.h"
#include "test_parser.h"
#include "test_semantic.h"
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

    /* Parser tests */
    fputc('\n', stdout);
    fprintf(stdout, BOLD LINE "PARSER TESTS" LINE ANSI_RESET "\n"); 
    uint total_parser = 0;
    uint crash_parser = 0;
    uint pass_parser = run_parser_tests(&total_parser, &crash_parser);
    print_test_summary("Parser", total_parser, pass_parser, crash_parser);
    
    total_tests += total_parser;
    total_pass += pass_parser;
    total_crash += crash_parser;

    /* Semantic tests */
    fputc('\n', stdout);
    fprintf(stdout, BOLD LINE "SEMANTIC TESTS" LINE ANSI_RESET "\n");
    uint total_semantic = 0;
    uint crash_semantic = 0;
    uint pass_semantic = run_semantic_tests(&total_semantic, &crash_semantic);
    print_test_summary("Semantic", total_semantic, pass_semantic, crash_semantic);

    total_tests += total_semantic;
    total_pass += pass_semantic;
    total_crash += crash_semantic;
    
    /* Overall summary */
    fputc('\n', stdout);
    print_overall_summary(total_tests, total_pass, total_crash);
    
    return EXIT_SUCCESS;
}
