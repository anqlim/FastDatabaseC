#include <gtest/gtest.h>

#include <sstream>

#include "database.h"
#include "exception.h"

TEST(DatabaseInsert, DuplicateKeyThrows) {
    Database db;
    Number first("81234567890");
    Number second("81234567890");
    std::string name = "\"Ivan\"";

    db.insert(first, name);

    EXPECT_THROW(db.insert(second, name), AVLTreeError);
}

TEST(DatabaseInsert, DuplicateKeyMessage) {
    Database db;
    Number first("81234567890");
    Number second("81234567890");
    std::string name = "\"Ivan\"";
    db.insert(first, name);

    try {
        db.insert(second, name);
        FAIL() << "expected AVLTreeError";
    } catch (const AVLTreeError& error) {
        EXPECT_STREQ(error.what(), "key exists");
    }
}

TEST(DatabaseInsert, FindAfterInsert) {
    Database db;
    Number key("81234567890");
    std::string name = "\"Petrov E.E.\"";

    db.insert(key, name);

    EXPECT_EQ(db.find(key), "\"Petrov E.E.\"");
}

TEST(DatabaseDelete, SuccessThenMissing) {
    Database db;
    Number key("81234567890");
    std::string name = "\"Anna\"";

    db.insert(key, name);
    db.del(key);

    EXPECT_EQ(db.find(key), "not_found");
    EXPECT_THROW(db.del(key), AVLTreeError);
}

TEST(DatabaseDelete, MissingKeyThrows) {
    Database db;
    Number key("+71234567890");

    EXPECT_THROW(db.del(key), AVLTreeError);
}

TEST(DatabaseDelete, MissingKeyMessage) {
    Database db;
    Number key("+71234567890");

    try {
        db.del(key);
        FAIL() << "expected AVLTreeError";
    } catch (const AVLTreeError& error) {
        EXPECT_STREQ(error.what(), "not found");
    }
}

TEST(DatabaseFind, MissingKeyReturnsNotFound) {
    Database db;
    Number key("81234567890");

    EXPECT_EQ(db.find(key), "not_found");
}

TEST(DatabaseTree, NullOnEmpty) {
    Database db;
    std::ostringstream out;

    db.tree(out);

    EXPECT_EQ(out.str(), "tree:(null)\n");
}

TEST(DatabaseTree, PrintsSingleRecord) {
    Database db;
    Number key("81234567890");
    std::string name = "\"Anna\"";
    db.insert(key, name);

    std::ostringstream out;
    db.tree(out);

    EXPECT_EQ(out.str(), "tree:(\"81234567890\")\n");
}

TEST(DatabaseTree, PrintsTwoRecords) {
    Database db;
    Number first("87451002823");
    Number second("86244259344");
    std::string name1 = "\"First\"";
    std::string name2 = "\"Second\"";
    db.insert(first, name1);
    db.insert(second, name2);

    std::ostringstream out;
    db.tree(out);

    EXPECT_EQ(out.str(), "tree:((\"86244259344\"),\"87451002823\",(null))\n");
}

TEST(DatabaseFind, Plus7Key) {
    Database db;
    Number key("+75529719054");
    std::string name = "\"Oleg\"";
    db.insert(key, name);

    EXPECT_EQ(db.find(key), "\"Oleg\"");
}

TEST(DatabaseInsert, InvalidNumberNotInserted) {
    Database db;
    EXPECT_THROW(
        {
            Number bad("invalid");
            std::string name = "\"X\"";
            db.insert(bad, name);
        },
        AVLTreeError);
    Number probe("81234567890");
    EXPECT_EQ(db.find(probe), "not_found");
}
