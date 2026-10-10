#include "parser.h"

#include <cstring>
#include <istream>
#include <string>

#include "exception.h"

namespace {

size_t skipSpaces(const std::string& s, size_t pos) {
    while (pos < s.size() && s[pos] == ' ') {
        ++pos;
    }
    return pos;
}

bool readQuoted(const std::string& s, size_t pos, std::string& inner, size_t& endPos) {
    pos = skipSpaces(s, pos);
    if (pos >= s.size() || s[pos] != '"') {
        return false;
    }
    const size_t close = s.find('"', pos + 1);
    if (close == std::string::npos) {
        return false;
    }
    inner = s.substr(pos + 1, close - pos - 1);
    endPos = close + 1;
    return true;
}

bool readName(const std::string& s, size_t pos, std::string& name, size_t& endPos) {
    pos = skipSpaces(s, pos);
    if (pos >= s.size() || s[pos] != '"') {
        return false;
    }

    int slashes = 0;
    size_t length = 0;
    for (;; ++length) {
        const char c = s[pos + 1 + length];
        if ((c == '"' || c == '\0') && (slashes % 2 == 0)) {
            break;
        }
        if (c == '\\') {
            ++slashes;
        } else {
            slashes = 0;
        }
    }
    if (pos + 1 + length >= s.size() || s[pos + 1 + length] != '"') {
        return false;
    }

    name = s.substr(pos, length + 2);
    endPos = pos + 1 + length + 1;
    return true;
}

void handleInsert(Database& db, std::ostream& output, const std::string& args) {
    std::string phoneRaw;
    size_t pos = 0;
    if (!readQuoted(args, pos, phoneRaw, pos)) {
        output << "incorrect\n";
        return;
    }

    std::string name;
    size_t end = 0;
    if (!readName(args, pos, name, end)) {
        output << "incorrect\n";
        return;
    }
    if (skipSpaces(args, end) != args.size()) {
        output << "incorrect\n";
        return;
    }

    try {
        Number number(phoneRaw);
        db.insert(number, name);
        output << "insert:success\n";
    } catch (const AVLTreeError& e) {
        if (std::strcmp(e.what(), "key exists") == 0) {
            output << "insert:e_exist\n";
        } else {
            output << "incorrect\n";
        }
    }
}

void handleFind(Database& db, std::ostream& output, const std::string& args) {
    std::string phoneRaw;
    size_t pos = 0;
    if (!readQuoted(args, pos, phoneRaw, pos)) {
        output << "incorrect\n";
        return;
    }
    if (skipSpaces(args, pos) != args.size()) {
        output << "incorrect\n";
        return;
    }

    try {
        Number number(phoneRaw);
        output << "find:" << db.find(number) << "\n";
    } catch (const AVLTreeError&) {
        output << "incorrect\n";
    }
}

void handleDelete(Database& db, std::ostream& output, const std::string& args) {
    std::string phoneRaw;
    size_t pos = 0;
    if (!readQuoted(args, pos, phoneRaw, pos)) {
        output << "incorrect\n";
        return;
    }
    if (skipSpaces(args, pos) != args.size()) {
        output << "incorrect\n";
        return;
    }

    try {
        Number number(phoneRaw);
        db.del(number);
        output << "delete:success\n";
    } catch (const AVLTreeError& e) {
        if (std::strcmp(e.what(), "not found") == 0) {
            output << "delete:not_found\n";
        } else {
            output << "incorrect\n";
        }
    }
}

void handleTree(Database& db, std::ostream& output, const std::string& args) {
    if (skipSpaces(args, 0) != args.size()) {
        output << "incorrect\n";
        return;
    }
    db.tree(output);
}

}  // namespace

Parser::Parser(std::ostream& output, Database& db) : output_(output), db_(db) {}

void Parser::parse(std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.compare(0, 6, "insert") == 0) {
            handleInsert(db_, output_, line.substr(6));
        } else if (line.compare(0, 4, "find") == 0) {
            handleFind(db_, output_, line.substr(4));
        } else if (line.compare(0, 6, "delete") == 0) {
            handleDelete(db_, output_, line.substr(6));
        } else if (line.compare(0, 4, "tree") == 0) {
            handleTree(db_, output_, line.substr(4));
        } else {
            output_ << "incorrect\n";
        }
    }
}
