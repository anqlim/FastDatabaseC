#include <gtest/gtest.h>

#include "exception.h"
#include "record.h"

using Number = Record::Number;

TEST(NumberConstructor, Accepts8Prefix) {
    Number number("81234567890");
    EXPECT_EQ(number.prefix(), '8');
    EXPECT_EQ(number.digits(), 1234567890ULL);
}

TEST(NumberConstructor, AcceptsPlus7Prefix) {
    Number number("+71234567890");
    EXPECT_EQ(number.prefix(), '7');
    EXPECT_EQ(number.digits(), 1234567890ULL);
}

TEST(NumberConstructor, ThrowsOnWrongLength) {
    EXPECT_THROW(Number("812345678"), AVLTreeError);
    EXPECT_THROW(Number("812345678901"), AVLTreeError);
    EXPECT_THROW(Number("+7123456789"), AVLTreeError);
}

TEST(NumberConstructor, ThrowsOnBadPrefix) {
    EXPECT_THROW(Number("91234567890"), AVLTreeError);
    EXPECT_THROW(Number("+81234567890"), AVLTreeError);
    EXPECT_THROW(Number("71234567890"), AVLTreeError);
}

TEST(NumberConstructor, ThrowsOnNonDigit) {
    EXPECT_THROW(Number("8123456789a"), AVLTreeError);
    EXPECT_THROW(Number("+7123456789x"), AVLTreeError);
}

TEST(NumberConstructor, IncorrectNumberMessage) {
    try {
        Number number("not-a-phone");
        FAIL() << "expected AVLTreeError";
    } catch (const AVLTreeError& error) {
        EXPECT_STREQ(error.what(), "incorrect number");
    }
}
