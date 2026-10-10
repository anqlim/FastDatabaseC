#pragma once

#include <iosfwd>
#include <string>

#include "database.h"

class Parser {
public:
    Parser(std::ostream& output, Database& db);

    void parse(std::istream& input);

private:
    std::ostream& output_;
    Database& db_;
};
