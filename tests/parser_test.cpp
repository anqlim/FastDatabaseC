#include <gtest/gtest.h>

#include "test_helpers.h"

TEST(Parser, InsertSuccess) {
    EXPECT_EQ(runParser("insert \"81234567890\" \"Ivan\"\n"), "insert:success\n");
}

TEST(Parser, InsertDuplicate) {
    const std::string input =
        "insert \"81234567890\" \"Ivan\"\n"
        "insert \"81234567890\" \"Petr\"\n";
    EXPECT_EQ(runParser(input), "insert:success\ninsert:e_exist\n");
}

TEST(Parser, InsertIncorrectPhone) {
    EXPECT_EQ(runParser("insert \"bad\" \"Ivan\"\n"), "incorrect\n");
}

TEST(Parser, InsertMissingName) {
    EXPECT_EQ(runParser("insert \"81234567890\"\n"), "incorrect\n");
}

TEST(Parser, InsertExtraTokens) {
    EXPECT_EQ(runParser("insert \"81234567890\" \"Ivan\" extra\n"), "incorrect\n");
}

TEST(Parser, FindSuccessAndNotFound) {
    const std::string input =
        "insert \"81234567890\" \"Ivan\"\n"
        "find \"81234567890\"\n"
        "find \"81234567891\"\n";
    EXPECT_EQ(runParser(input), "insert:success\nfind:\"Ivan\"\nfind:not_found\n");
}

TEST(Parser, FindIncorrect) {
    EXPECT_EQ(runParser("find \"81234567890\" junk\n"), "incorrect\n");
    EXPECT_EQ(runParser("find bad\n"), "incorrect\n");
}

TEST(Parser, DeleteSuccessAndNotFound) {
    const std::string input =
        "insert \"81234567890\" \"Ivan\"\n"
        "delete \"81234567890\"\n"
        "delete \"81234567890\"\n";
    EXPECT_EQ(runParser(input),
              "insert:success\ndelete:success\ndelete:not_found\n");
}

TEST(Parser, TreeEmptyAndIncorrect) {
    EXPECT_EQ(runParser("tree\n"), "tree:(null)\n");
    EXPECT_EQ(runParser("tree extra\n"), "incorrect\n");
}

TEST(Parser, UnknownCommand) {
    EXPECT_EQ(runParser("list\n"), "incorrect\n");
}

TEST(Parser, SkipsEmptyLines) {
    EXPECT_EQ(runParser("\ninsert \"81234567890\" \"Ivan\"\n\n"), "insert:success\n");
}

TEST(Parser, NameWithEscapedQuote) {
    EXPECT_EQ(runParser("insert \"81234567890\" \"Iva\\\"n\"\n"), "insert:success\n");
}

TEST(Parser, InsertPlus7) {
    EXPECT_EQ(runParser("insert \"+79001234567\" \"Ivanov I.I\"\n"), "insert:success\n");
}

TEST(Parser, InsertUnquotedPhone) {
    EXPECT_EQ(runParser("insert 89001234566 \"Name\"\n"), "incorrect\n");
}

TEST(Parser, InsertUnclosedName) {
    EXPECT_EQ(runParser("insert \"81234567890\" \"broken\n"), "incorrect\n");
}

TEST(Parser, TreeAfterTwoInserts) {
    const std::string input =
        "insert \"87451002823\" \"First\"\n"
        "insert \"86244259344\" \"Second\"\n"
        "tree\n";
    EXPECT_EQ(runParser(input),
              "insert:success\ninsert:success\n"
              "tree:((\"86244259344\"),\"87451002823\",(null))\n");
}

TEST(Parser, FindAfterEscapedNameInsert) {
    const std::string input =
        "insert \"81234567890\" \"Iva\\\"n\"\n"
        "find \"81234567890\"\n";
    EXPECT_EQ(runParser(input), "insert:success\nfind:\"Iva\\\"n\"\n");
}

TEST(Parser, DeleteIncorrectArgs) {
    EXPECT_EQ(runParser("delete \"81234567890\" extra\n"), "incorrect\n");
}

TEST(Parser, InsertIncorrectPhoneOnDuplicatePath) {
    EXPECT_EQ(runParser("insert \"81234567890\" \"A\"\ninsert \"bad\" \"B\"\n"),
              "insert:success\nincorrect\n");
}
