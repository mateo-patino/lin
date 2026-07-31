#include "test_assert.h"
#include "test_helpers.h"

void assert_true_failed(const char *expr, const char *file, int line, const char *func) {
    perr(BOLD "%s " ANSI_RED "FAILED" ANSI_RESET "\n", func);
    perr("   Assertion failed: %s\n", expr);
    perr("   at %s:%i\n", file, line);
    perr("   in %s\n", func);
}
