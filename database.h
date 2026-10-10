#pragma once

#include <iosfwd>
#include <string>

#include "record.h"
#include "avl_tree.h"

using Number = Record::Number;

class Database {
public:
    Database();

    void insert(const Number& number, const std::string& name);
    std::string find(const Number& number);
    void del(const Number& number);
    void tree(std::ostream& output);

private:
    AVLTree tree_;
};
