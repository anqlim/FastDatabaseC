#include <gtest/gtest.h>

#include "avl_tree.h"
#include "test_helpers.h"

TEST(AVLTree, EmptyTree) {
    AVLTree tree;

    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.formatTree(), "(null)");
    EXPECT_EQ(tree.findName(1234567890ULL), "not_found");
}

TEST(AVLTree, InsertAndFind) {
    AVLTree tree;

    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"Anna\"")));
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.findName(1234567890ULL), "\"Anna\"");

    ASSERT_TRUE(tree.insert(makeRecord("+71234567891", "\"Boris\"")));
    EXPECT_EQ(tree.findName(1234567891ULL), "\"Boris\"");
}

TEST(AVLTree, InsertDuplicateReturnsFalse) {
    AVLTree tree;

    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"First\"")));
    EXPECT_FALSE(tree.insert(makeRecord("81234567890", "\"Second\"")));
    EXPECT_EQ(tree.findName(1234567890ULL), "\"First\"");
}

TEST(AVLTree, RemoveExistingAndMissing) {
    AVLTree tree;
    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"Anna\"")));

    Number key("81234567890");
    Record lookup(key, "");
    EXPECT_TRUE(tree.remove(lookup));
    EXPECT_TRUE(tree.empty());

    EXPECT_FALSE(tree.remove(lookup));
}

TEST(AVLTree, FormatTreeSingleNode) {
    AVLTree tree;
    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"Anna\"")));

    EXPECT_EQ(tree.formatTree(), "(\"81234567890\")");
}

TEST(AVLTree, BalancingAfterOrderedInsert) {
    AVLTree tree;

    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"A\"")));
    ASSERT_TRUE(tree.insert(makeRecord("81234567891", "\"B\"")));
    ASSERT_TRUE(tree.insert(makeRecord("81234567892", "\"C\"")));

    EXPECT_EQ(tree.findName(1234567890ULL), "\"A\"");
    EXPECT_EQ(tree.findName(1234567891ULL), "\"B\"");
    EXPECT_EQ(tree.findName(1234567892ULL), "\"C\"");
    EXPECT_NE(tree.formatTree(), "(null)");
}

TEST(AVLTree, FormatTreeTwoNodes) {
    AVLTree tree;
    ASSERT_TRUE(tree.insert(makeRecord("87451002823", "\"Root\"")));
    ASSERT_TRUE(tree.insert(makeRecord("86244259344", "\"Left\"")));

    EXPECT_EQ(tree.formatTree(), "((\"86244259344\"),\"87451002823\",(null))");
}

TEST(AVLTree, FormatTreeUsesPlus7Prefix) {
    AVLTree tree;
    ASSERT_TRUE(tree.insert(makeRecord("+75529719054", "\"X\"")));

    EXPECT_EQ(tree.formatTree(), "(\"+75529719054\")");
}

TEST(AVLTree, RemoveMiddleNode) {
    AVLTree tree;
    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"A\"")));
    ASSERT_TRUE(tree.insert(makeRecord("81234567891", "\"B\"")));
    ASSERT_TRUE(tree.insert(makeRecord("81234567892", "\"C\"")));

    Number mid("81234567891");
    Record key(mid, "");
    ASSERT_TRUE(tree.remove(key));

    EXPECT_EQ(tree.findName(1234567891ULL), "not_found");
    EXPECT_EQ(tree.findName(1234567890ULL), "\"A\"");
    EXPECT_EQ(tree.findName(1234567892ULL), "\"C\"");
    EXPECT_FALSE(tree.empty());
}

TEST(AVLTree, DescendingInsertStillFindsAll) {
    AVLTree tree;
    ASSERT_TRUE(tree.insert(makeRecord("81234567892", "\"C\"")));
    ASSERT_TRUE(tree.insert(makeRecord("81234567891", "\"B\"")));
    ASSERT_TRUE(tree.insert(makeRecord("81234567890", "\"A\"")));

    EXPECT_EQ(tree.findName(1234567890ULL), "\"A\"");
    EXPECT_EQ(tree.findName(1234567891ULL), "\"B\"");
    EXPECT_EQ(tree.findName(1234567892ULL), "\"C\"");
}
