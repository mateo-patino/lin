#include "test_assert.h"
#include "types/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"

#include <assert.h>

/*
* Helper for creating an AST from an input string.
*
* The test suite or process sandbox will abort if `expr` produces an invalid
* tokens array.
*
* Returns a pointer to the root of the AST.
*/

static node_t *create_ast_from_string(const char *expr, parse_status *status) {

    /* Tokenization should work correctly */
    size_t token_count;
    tokens_status tok_status;
    token_t *tokens = create_tokens_from_string(expr, &token_count, &tok_status);
    assert(tok_status == TOKENS_OK);

    /* This is the part we're interested in in these tests */
    parse_status par_status;
    ast_t *ast = create_ast_from_tokens(tokens, token_count, &par_status);

    if (status) {
        *status = par_status;
    }

    if (!ast) {
        fully_free_tokens(tokens, token_count);
        return NULL;
    }

    return ast->root;
}


static bool test_valid_ast_easy(void) { 
    node_t *root;
    parse_status st;
    
    root = create_ast_from_string("1 + 1", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_SCALAR_NODE(root->right, 1);

    return true;
}


static const test_case_t parser_tests[] = {
    TEST(test_valid_ast_easy)
};


uint run_parser_tests(uint *total, uint *crashes) {
    uint total_parser_tests = ARRAY_LEN(parser_tests);

    int result;
    int crash_count = 0;
    int pass_count = 0;
    int signum = -1;
    for (uint i = 0; i < total_parser_tests; i++) {
        result = run_in_sandbox(parser_tests + i, &signum);

        switch (result) {
            case -1:
                break;
            case 0:
                print_success(parser_tests + i); 
                pass_count++;
                break;
            case 1:
                /* No printing if tests fails, done in the ASSERT macro */
                break;
            case 2:
                print_crash(parser_tests + i, signum); 
                crash_count++;
                break;
            default:
                break;
         }
    }

    if (crashes) { *crashes = crash_count; }
    if (total) { *total = total_parser_tests; }

    return pass_count;
}
