#pragma once

#include <memory>
#include <sstream>
#include <string>

#include "database.h"
#include "parser.h"

inline std::string runParser(const std::string& input) {
    std::istringstream in(input);
    std::ostringstream out;
    Database db;
    Parser parser(out, db);
    parser.parse(in);
    return out.str();
}

inline std::unique_ptr<Record> makeRecord(const char* phone, const char* name) {
    Number number(phone);
    return std::make_unique<Record>(number, name);
}
