#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <stdbool.h>

/* Assert `expr` is true */
void assert_true_failed(const char *expr, const char *file, int line, const char *func);
#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            assert_true_failed(#x, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)


#endif
