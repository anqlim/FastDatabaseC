#include "exception.h"

AVLTreeError::AVLTreeError(const char* message) : message_(message) {}

const char* AVLTreeError::what() const noexcept {
    return message_.c_str();
}
