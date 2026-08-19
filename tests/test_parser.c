#include "test_assert.h"
#include "types/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "errorprinter.h"

#include <assert.h>


/*
* Helper for creating an AST from an input string.
*
* The test suite or process sandbox will abort if `expr` produces an invalid
* tokens array.
*
* Returns a pointer to the root of the AST.
*
* 
* KNOWN BUG: this function leaves some reachable bytes due to the tokens being allocated
* but never freed in the tests. 
*/
static node_t *create_ast_from_string(const char *expr, parse_status *status) {

    /* Tokenization should work correctly */
    size_t token_count;
    tokens_status tok_status;
    token_t *tokens = create_tokens_from_string(expr, &token_count, &tok_status);
    assert(tok_status == TOKENS_OK);

    /* This is the part we're interested in in these tests */
    parse_status par_status = PARSE_OK;
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

    root = create_ast_from_string("1 - 1 + 6.7", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_OPERATOR_NODE(root->left, SUB);
    ASSERT_SCALAR_NODE(root->right, 6.7);
    ASSERT_SCALAR_NODE(root->left->left, 1);
    ASSERT_SCALAR_NODE(root->left->right, 1);

    root = create_ast_from_string("1 + 1 * 2", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_OPERATOR_NODE(root->right, MUL);
    ASSERT_SCALAR_NODE(root->right->left, 1);
    ASSERT_SCALAR_NODE(root->right->right, 2);

    const scalar_t entries[] = { 1, 2, 3, 4 };
    const matrix_test_case_t matrix = { "2x2 1 2 3 4", 2, 2, entries };

    root = create_ast_from_string("det 2x2 1 2 3 4", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, DET);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_MATRIX_NODE(root->right, &matrix);

    root = create_ast_from_string("rref 2x2 1 2 3 4", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, RREF);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_MATRIX_NODE(root->right, &matrix);
    
    root = create_ast_from_string("inv 2x2 1 2 3 4", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, INV);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_MATRIX_NODE(root->right, &matrix);

    return true;
}


bool test_valid_ast_medium(void) {
    node_t *root;
    parse_status st;

    root = create_ast_from_string("1 + 2 * 3 / 4", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_OPERATOR_NODE(root->right, DIV);
    ASSERT_OPERATOR_NODE(root->right->left, MUL);
    ASSERT_SCALAR_NODE(root->right->left->left, 2);
    ASSERT_SCALAR_NODE(root->right->left->right, 3);
    ASSERT_SCALAR_NODE(root->right->right, 4);

    root = create_ast_from_string("( 1 + 2 ) * 3 / 4", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, DIV);
    ASSERT_SCALAR_NODE(root->right, 4);
    ASSERT_OPERATOR_NODE(root->left, MUL);
    ASSERT_SCALAR_NODE(root->left->right, 3);
    ASSERT_OPERATOR_NODE(root->left->left, ADD);
    ASSERT_SCALAR_NODE(root->left->left->left, 1);
    ASSERT_SCALAR_NODE(root->left->left->right, 2);

    root = create_ast_from_string("( 1 + 2 ) * ( 3 / 4 )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, MUL);
    ASSERT_OPERATOR_NODE(root->left, ADD);
    ASSERT_OPERATOR_NODE(root->right, DIV);
    ASSERT_SCALAR_NODE(root->left->left, 1);
    ASSERT_SCALAR_NODE(root->left->right, 2);
    ASSERT_SCALAR_NODE(root->right->left, 3);
    ASSERT_SCALAR_NODE(root->right->right, 4);


    const scalar_t entries[] = { 1, 0, 0, 1};
    const matrix_test_case_t matrix = { "2x2 1 0 0 1", 2, 2, entries };

    root = create_ast_from_string("1 + det 2x2 1 0 0 1", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_OPERATOR_NODE(root->right, DET);
    ASSERT_TRUE(root->right->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right, &matrix);

    /* should behave as 1 + ( det 2x2 1 0 0 1 * 2 ) */
    root = create_ast_from_string("1 + det 2x2 1 0 0 1 * 2", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_OPERATOR_NODE(root->right, MUL);
    ASSERT_OPERATOR_NODE(root->right->left, DET);
    ASSERT_SCALAR_NODE(root->right->right, 2);
    ASSERT_TRUE(root->right->left->left == NULL);
    ASSERT_MATRIX_NODE(root->right->left->right, &matrix);

    /* should behave as ( det 2x2 1 0 0 1 ) * ( inv 2x2 1 0 0 1 ) */
    root = create_ast_from_string("det 2x2 1 0 0 1 * inv 2x2 1 0 0 1", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, MUL);
    ASSERT_OPERATOR_NODE(root->left, DET);
    ASSERT_OPERATOR_NODE(root->right, INV);
    ASSERT_TRUE(root->left->left == NULL);
    ASSERT_MATRIX_NODE(root->left->right, &matrix);
    ASSERT_TRUE(root->right->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right, &matrix);

    /* should behave as rref ( inv ( 67 * 2x2 1 0 0 1 ) ) */
    root = create_ast_from_string("rref inv ( 67 * 2x2 1 0 0 1 )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, RREF);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_OPERATOR_NODE(root->right, INV);
    ASSERT_TRUE(root->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->right->right, MUL);
    ASSERT_SCALAR_NODE(root->right->right->left, 67);
    ASSERT_MATRIX_NODE(root->right->right->right, &matrix);
    
    return true;
}


static bool test_valid_ast_hard(void) {
    node_t *root;
    parse_status st;

    const scalar_t entries_a[] = { 1, 0, 0, 1 };
    const scalar_t entries_b[] = { 2, 1, 1, 2 };
    const scalar_t entries_c[] = { 1, 2, 3, 5 };
    const scalar_t entries_d[] = { 2, 0, 0, 3 };
    const scalar_t entries_e[] = { 1, 2, 3, 4 };
    const scalar_t entries_f[] = { 3, 1, 1, 1 };

    const matrix_test_case_t matrix_a = { "2x2 1 0 0 1", 2, 2, entries_a };
    const matrix_test_case_t matrix_b = { "2x2 2 1 1 2", 2, 2, entries_b };
    const matrix_test_case_t matrix_c = { "2x2 1 2 3 5", 2, 2, entries_c };
    const matrix_test_case_t matrix_d = { "2x2 2 0 0 3", 2, 2, entries_d };
    const matrix_test_case_t matrix_e = { "2x2 1 2 3 4", 2, 2, entries_e };
    const matrix_test_case_t matrix_f = { "2x2 3 1 1 1", 2, 2, entries_f };

    /*
    * Should behave as:
    *
    * ( ( det ( rref ( ( A + B ) * inv ( C ) ) ) + det ( inv ( D ) ) )
    *   - det ( rref ( E ) ) )
    * / ( 1 + det ( inv ( F ) ) )
    */
    root = create_ast_from_string("( ( det ( rref ( ( 2x2 1 0 0 1 + 2x2 2 1 1 2 ) * inv ( 2x2 1 2 3 5 ) ) ) + det ( inv ( 2x2 2 0 0 3 ) ) ) - det ( rref ( 2x2 1 2 3 4 ) ) ) / ( 1 + det ( inv ( 2x2 3 1 1 1 ) ) )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, DIV);
    ASSERT_OPERATOR_NODE(root->left, SUB);
    ASSERT_OPERATOR_NODE(root->right, ADD);

    ASSERT_OPERATOR_NODE(root->left->left, ADD);
    ASSERT_OPERATOR_NODE(root->left->right, DET);
    ASSERT_SCALAR_NODE(root->right->left, 1);
    ASSERT_OPERATOR_NODE(root->right->right, DET);

    ASSERT_OPERATOR_NODE(root->left->left->left, DET);
    ASSERT_OPERATOR_NODE(root->left->left->right, DET);
    ASSERT_TRUE(root->left->left->left->left == NULL);
    ASSERT_OPERATOR_NODE(root->left->left->left->right, RREF);
    ASSERT_TRUE(root->left->left->left->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->left->left->left->right->right, MUL);
    ASSERT_OPERATOR_NODE(root->left->left->left->right->right->left, ADD);
    ASSERT_OPERATOR_NODE(root->left->left->left->right->right->right, INV);
    ASSERT_MATRIX_NODE(root->left->left->left->right->right->left->left, &matrix_a);
    ASSERT_MATRIX_NODE(root->left->left->left->right->right->left->right, &matrix_b);
    ASSERT_TRUE(root->left->left->left->right->right->right->left == NULL);
    ASSERT_MATRIX_NODE(root->left->left->left->right->right->right->right, &matrix_c);

    ASSERT_TRUE(root->left->left->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->left->left->right->right, INV);
    ASSERT_TRUE(root->left->left->right->right->left == NULL);
    ASSERT_MATRIX_NODE(root->left->left->right->right->right, &matrix_d);

    ASSERT_TRUE(root->left->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->left->right->right, RREF);
    ASSERT_TRUE(root->left->right->right->left == NULL);
    ASSERT_MATRIX_NODE(root->left->right->right->right, &matrix_e);

    ASSERT_TRUE(root->right->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->right->right->right, INV);
    ASSERT_TRUE(root->right->right->right->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right->right->right, &matrix_f);

    /*
    * Should behave as:
    *
    * rref ( inv ( ( A + ( B * inv ( C ) ) )
    *              * ( rref ( D ) - E ) ) )
    */
    root = create_ast_from_string("rref ( inv ( ( 2x2 1 0 0 1 + ( 2x2 2 1 1 2 * inv ( 2x2 1 2 3 5 ) ) ) * ( rref ( 2x2 2 0 0 3 ) - 2x2 1 2 3 4 ) ) )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, RREF);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_OPERATOR_NODE(root->right, INV);
    ASSERT_TRUE(root->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->right->right, MUL);

    ASSERT_OPERATOR_NODE(root->right->right->left, ADD);
    ASSERT_OPERATOR_NODE(root->right->right->right, SUB);
    ASSERT_MATRIX_NODE(root->right->right->left->left, &matrix_a);
    ASSERT_OPERATOR_NODE(root->right->right->left->right, MUL);
    ASSERT_MATRIX_NODE(root->right->right->left->right->left, &matrix_b);
    ASSERT_OPERATOR_NODE(root->right->right->left->right->right, INV);
    ASSERT_TRUE(root->right->right->left->right->right->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right->left->right->right->right, &matrix_c);

    ASSERT_OPERATOR_NODE(root->right->right->right->left, RREF);
    ASSERT_TRUE(root->right->right->right->left->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right->right->left->right, &matrix_d);
    ASSERT_MATRIX_NODE(root->right->right->right->right, &matrix_e);

    return true;
}


static bool test_invalid_ast_easy(void) {
    parse_status st;

    /* 
    * create_ast_from_tokens is the primary function in the parser module. It takes
    * the array of tokens by the lexer and produces an AST 
    */
    ASSERT_TRUE(create_ast_from_tokens(NULL, 0, NULL) == NULL);
    
    ASSERT_TRUE(create_ast_from_tokens(NULL, 0, &st) == NULL);
    ASSERT_TRUE(st == PARSE_INVALID_TOKENS);

    /*
    * create_ast_from_string is NOT part of the parser API. It is simply a helper in
    * this file that grabs a string, tokenizes it, and feeds the tokens into the parser. 
    * Internally, it uses create_tokens_from_string (lexer) and create_ast_from_tokens (parser)
    */
    clear_error();
    create_ast_from_string("+", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("1 + ", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("+ +", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("1 2 3 4 5 6 7 8 9", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("det", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("det ( inv (  ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("add sub div mul det inv rref", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
     
    return true;
}


/* More realistic expresions */
static bool test_invalid_ast_medium(void) {
    parse_status st;

    clear_error();
    create_ast_from_string("1 det 2x2 1 2 3 4", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);


    clear_error();
    create_ast_from_string("1 2x2 1 2 3 4", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("2x2 1 2 3 4 67", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("1 + ( 3 * inv 2x2 1 2 3 4", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    clear_error();
    create_ast_from_string("det 2x2 1 1 1 1 * 6 + 7 )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    clear_error();
    create_ast_from_string("1 + 1 (  )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    clear_error();
    create_ast_from_string("1 + 1 + ( ( ( 1 ) + 1  )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    return true;
}

/* Expressions where nearly everything is valid except the last few subtrees. Here we test
* that an error can be correctly caught and raised deep inside the recursion tree when most 
* of the AST has been built. */
static bool test_invalid_ast_hard(void) {
    parse_status st;

    /* There is a extraneous 67 to the left of a 2x2 matrix at the end of the expression */
    clear_error();
    create_ast_from_string("( ( det ( rref ( ( 2x2 1 0 0 1 + 2x2 2 1 1 2 ) * inv ( 2x2 1 2 3 5 ) ) ) + det ( inv ( 2x2 2 0 0 3 ) ) ) - det ( rref ( 2x2 1 2 3 4 ) ) ) / ( 1 + det ( inv ( 67 2x2 3 1 1 1 ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    /* There is a extraneous 67 to the left of the rightmost RREF */
    clear_error();
    create_ast_from_string("rref ( inv ( ( 2x2 1 0 0 1 + ( 2x2 2 1 1 2 * inv ( 2x2 1 2 3 5 ) ) ) * ( 67 rref ( 2x2 2 0 0 3 ) - 2x2 1 2 3 4 ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    /* There is a extraneous + to the left of the rightmost + */
    clear_error();
    create_ast_from_string("( ( det ( rref ( ( 2x2 1 0 0 1 + 2x2 2 1 1 2 ) * inv ( 2x2 1 2 3 5 ) ) ) + det ( inv ( 2x2 2 0 0 3 ) ) ) - det ( rref ( 2x2 1 2 3 4 ) ) ) / ( 1 + + det ( inv ( 67 2x2 3 1 1 1 ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    /* There is a extraneous + to the left of the rightmost RREF */
    clear_error();
    create_ast_from_string("rref ( inv ( ( 2x2 1 0 0 1 + ( 2x2 2 1 1 2 * inv ( 2x2 1 2 3 5 ) ) ) * ( + rref ( 2x2 2 0 0 3 ) - 2x2 1 2 3 4 ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    /* There is a extraneous matrix to the right of the rightmost matrix */
    clear_error();
    create_ast_from_string("( ( det ( rref ( ( 2x2 1 0 0 1 + 2x2 2 1 1 2 ) * inv ( 2x2 1 2 3 5 ) ) ) + det ( inv ( 2x2 2 0 0 3 ) ) ) - det ( rref ( 2x2 1 2 3 4 ) ) ) / ( 1 + + det ( inv ( 67 2x2 3 1 1 1 2x2 6 7 6 7 ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    /* There is a extraneous matrix to the left of the rightmost RREF */
    clear_error();
    create_ast_from_string("rref ( inv ( ( 2x2 1 0 0 1 + ( 2x2 2 1 1 2 * inv ( 2x2 1 2 3 5 ) ) ) * ( 2x2 1 2 3 4 rref ( 2x2 2 0 0 3 ) - 2x2 1 2 3 4 ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);
    
    /* There is a extra close parenthesis at the very end of the expression */
    clear_error();
    create_ast_from_string("( ( det ( rref ( ( 2x2 1 0 0 1 + 2x2 2 1 1 2 ) * inv ( 2x2 1 2 3 5 ) ) ) + det ( inv ( 2x2 2 0 0 3 ) ) ) - det ( rref ( 2x2 1 2 3 4 ) ) ) / ( 1 + + det ( inv ( 67 2x2 3 1 1 1 ) ) ) )", &st);
    ASSERT_TRUE(st != PARSE_OK);
    ASSERT_TRUE(has_error() == true);

    return true;
}


static bool test_operator_associativity(void) {
    node_t *root;
    parse_status st;

    root = create_ast_from_string("1 + 2 + 3", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_OPERATOR_NODE(root->left, ADD);
    ASSERT_SCALAR_NODE(root->left->left, 1);
    ASSERT_SCALAR_NODE(root->left->right, 2);
    ASSERT_SCALAR_NODE(root->right, 3);

    root = create_ast_from_string("10 - 3 - 2", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, SUB);
    ASSERT_OPERATOR_NODE(root->left, SUB);
    ASSERT_SCALAR_NODE(root->left->left, 10);
    ASSERT_SCALAR_NODE(root->left->right, 3);
    ASSERT_SCALAR_NODE(root->right, 2);

    root = create_ast_from_string("2 * 3 * 4", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, MUL);
    ASSERT_OPERATOR_NODE(root->left, MUL);
    ASSERT_SCALAR_NODE(root->left->left, 2);
    ASSERT_SCALAR_NODE(root->left->right, 3);
    ASSERT_SCALAR_NODE(root->right, 4);

    root = create_ast_from_string("24 / 6 / 2", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, DIV);
    ASSERT_OPERATOR_NODE(root->left, DIV);
    ASSERT_SCALAR_NODE(root->left->left, 24);
    ASSERT_SCALAR_NODE(root->left->right, 6);
    ASSERT_SCALAR_NODE(root->right, 2);

    const scalar_t entries[] = { 1, 0, 0, 1 };
    const matrix_test_case_t matrix = { "2x2 1 0 0 1", 2, 2, entries };

    root = create_ast_from_string("det inv rref 2x2 1 0 0 1", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, DET);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_OPERATOR_NODE(root->right, INV);
    ASSERT_TRUE(root->right->left == NULL);
    ASSERT_OPERATOR_NODE(root->right->right, RREF);
    ASSERT_TRUE(root->right->right->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right->right, &matrix);

    return true;
}

static bool test_valid_ast_parentheses(void) {
    node_t *root;
    parse_status st;

    root = create_ast_from_string("( 1 )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_SCALAR_NODE(root, 1);

    root = create_ast_from_string("( ( ( 1 ) ) )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_SCALAR_NODE(root, 1);

    root = create_ast_from_string("( 1 ) + ( 2 )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_SCALAR_NODE(root->right, 2);

    const scalar_t entries[] = { 1, 0, 0, 1 };
    const matrix_test_case_t matrix = { "2x2 1 0 0 1", 2, 2, entries };

    root = create_ast_from_string("( det ( inv ( 2x2 1 0 0 1 ) ) )", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_OPERATOR_NODE(root, DET);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_OPERATOR_NODE(root->right, INV);
    ASSERT_TRUE(root->right->left == NULL);
    ASSERT_MATRIX_NODE(root->right->right, &matrix);

    return true;
}


static bool test_invalid_ast_parentheses(void) {
    parse_status st;

    clear_error();
    create_ast_from_string("( )", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("1 + ( )", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("1 + 1 ( )", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("( 1 + 2", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string("1 + 2 )", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    create_ast_from_string(") 1 + 2 (", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    return true;
}

static bool test_parser_status_reset(void) {
    node_t *root;
    parse_status st;
    const scalar_t entries[] = { 1, 0, 0, 1 };
    const matrix_test_case_t matrix = { "2x2 1 0 0 1", 2, 2, entries };

    clear_error();
    create_ast_from_string("1 +", &st);
    ASSERT_TRUE(st == PARSE_INVALID_EXPRESSION);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    root = create_ast_from_string("1 + 1", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_TRUE(has_error() == false);
    ASSERT_OPERATOR_NODE(root, ADD);
    ASSERT_SCALAR_NODE(root->left, 1);
    ASSERT_SCALAR_NODE(root->right, 1);

    clear_error();
    create_ast_from_string("( 1 + 2", &st);
    ASSERT_TRUE(st == PARSE_UNBALANCED_PARENS);
    ASSERT_TRUE(has_error() == true);

    clear_error();
    root = create_ast_from_string("det 2x2 1 0 0 1", &st);
    ASSERT_TRUE(st == PARSE_OK);
    ASSERT_TRUE(has_error() == false);
    ASSERT_OPERATOR_NODE(root, DET);
    ASSERT_TRUE(root->left == NULL);
    ASSERT_MATRIX_NODE(root->right, &matrix);

    return true;
}


static const test_case_t parser_tests[] = {
    TEST(test_valid_ast_easy),
    TEST(test_valid_ast_medium),
    TEST(test_valid_ast_hard),
    TEST(test_invalid_ast_easy),
    TEST(test_invalid_ast_medium),
    TEST(test_invalid_ast_hard),
    TEST(test_operator_associativity),
    TEST(test_valid_ast_parentheses),
    TEST(test_invalid_ast_parentheses),
    TEST(test_parser_status_reset)
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
