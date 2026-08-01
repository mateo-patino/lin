#include "test_assert.h"
#include "test_helpers.h"
#include "test_lexer.h"
#include "types/token.h"
#include "lexer/lexer.h"

#include <stdbool.h>


static bool test_operator_lexer_valid(void) {
    operator_type op;

    ASSERT_TRUE(is_operator("add", &op));
    ASSERT_TRUE(op == MAT_ADD);
    ASSERT_TRUE(is_operator("plus", &op));
    ASSERT_TRUE(op == MAT_ADD);
    ASSERT_TRUE(is_operator("+", &op));
    ASSERT_TRUE(op == MAT_ADD);

    return true;
}

static const test_case_t lexer_tests[] = { 
    TEST(test_operator_lexer_valid) 
};



uint run_lexer_tests(uint *total, uint *crashes) {
    uint total_lexer_tests = ARRAY_LEN(lexer_tests);
    
    int result;
    int crash_count = 0;
    int pass_count = 0;
    int signum = -1;
    for (uint i = 0; i < total_lexer_tests; i++) {
        result = run_in_sandbox(lexer_tests + i, &signum);

        switch (result) {
            case -1:
                break;
            case 0:
                print_success(lexer_tests + i); 
                pass_count++;
                break;
            case 1:
                /* No printing if tests fails, done in the ASSERT macro */
                break;
            case 2:
                print_crash(lexer_tests + i, signum); 
                crash_count++;
                break;
            default:
                break;
         }
    }

    if (crashes) { *crashes = crash_count; }
    if (total) { *total = total_lexer_tests; }

    return pass_count;
}
