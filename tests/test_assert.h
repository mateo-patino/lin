#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <stdbool.h>

/* Assert `expr` is true */
#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            assert_true_failed(#x, __FILE__, __LINE__, __func__); \
            return false; \
        } \
    } while (0)


#endif
