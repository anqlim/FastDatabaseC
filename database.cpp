#include "database.h"

#include <memory>
#include <ostream>
#include <utility>

#include "exception.h"

Database::Database() = default;

void Database::insert(const Number& number, const std::string& name) {
    auto record = std::make_unique<Record>(number, name);
    if (!tree_.insert(std::move(record))) {
        throw AVLTreeError("key exists");
    }
}

std::string Database::find(const Number& number) {
    return tree_.findName(number.digits());
}

void Database::del(const Number& number) {
    Record key(number, "");
    if (!tree_.remove(key)) {
        throw AVLTreeError("not found");
    }
}

void Database::tree(std::ostream& output) {
    if (tree_.empty()) {
        output << "tree:(null)\n";
        return;
    }
    output << "tree:" << tree_.formatTree() << "\n";
}
