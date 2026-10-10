#pragma once

#include <exception>
#include <string>

class AVLTreeError : public std::exception {
public:
    explicit AVLTreeError(const char* message);
    const char* what() const noexcept override;

private:
    std::string message_;
};
