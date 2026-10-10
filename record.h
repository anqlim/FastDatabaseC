#pragma once

#include <cstdint>
#include <string>

struct Record {
    class Number {
    public:
        explicit Number(const std::string& number);

        char prefix() const { return prefix_; }
        std::uint64_t digits() const { return number_; }

    private:
        char prefix_;
        std::uint64_t number_;
    };

    Record(const Number& number, const std::string& name);

    Number number;
    std::string name;
};
