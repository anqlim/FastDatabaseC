#include <gtest/gtest.h>

#include "exception.h"

TEST(AVLTreeError, StoresMessage) {
    AVLTreeError error("custom message");
    EXPECT_STREQ(error.what(), "custom message");
}

TEST(AVLTreeError, KeyExistsAndNotFoundMessages) {
    EXPECT_STREQ(AVLTreeError("key exists").what(), "key exists");
    EXPECT_STREQ(AVLTreeError("not found").what(), "not found");
}
