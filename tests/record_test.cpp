#include <gtest/gtest.h>

#include "record.h"

TEST(Record, StoresNumberAndName) {
    Record::Number number("81234567890");
    const std::string name = "\"Petrov\"";

    Record record(number, name);

    EXPECT_EQ(record.number.prefix(), '8');
    EXPECT_EQ(record.number.digits(), 1234567890ULL);
    EXPECT_EQ(record.name, name);
}

TEST(Record, StoresPlus7Number) {
    Record::Number number("+71234567890");
    const std::string name = "\"Anna\"";

    Record record(number, name);

    EXPECT_EQ(record.number.prefix(), '7');
    EXPECT_EQ(record.number.digits(), 1234567890ULL);
    EXPECT_EQ(record.name, name);
}
