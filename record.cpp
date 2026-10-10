#include "record.h"

#include <string_view>

#include "exception.h"

namespace {

void parseNumber(std::string_view number, char& prefix, std::uint64_t& value) {
    const std::size_t len = number.size();
    auto end = number.end();
    auto it = number.begin();

    if (len == 11) {
        if (number[0] != '8') {
            throw AVLTreeError("incorrect number");
        }
        prefix = '8';
        ++it;
    } else if (len == 12) {
        if (number[0] != '+' || number[1] != '7') {
            throw AVLTreeError("incorrect number");
        }
        prefix = '7';
        it += 2;
    } else {
        throw AVLTreeError("incorrect number");
    }

    value = 0;
    for (int i = 0; i < 10; ++i) {
        if (it == end || *it < '0' || *it > '9') {
            throw AVLTreeError("incorrect number");
        }
        value = value * 10 + static_cast<std::uint64_t>(*it - '0');
        ++it;
    }
}

}  // namespace

Record::Number::Number(const std::string& number) {
    parseNumber(number, prefix_, number_);
}

Record::Record(const Number& number, const std::string& name)
    : number(number), name(name) {}
