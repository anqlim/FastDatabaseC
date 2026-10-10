#include "avl_tree.h"

#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <utility>

size_t AVLTree::getHeight(const std::unique_ptr<Node>& node) const {
    if (!node) {
        return 0;
    }
    return node->height;
}

void AVLTree::updateHeight(const std::unique_ptr<Node>& node) {
    if (node) {
        node->height = 1 + std::max(getHeight(node->leftTree), getHeight(node->rightTree));
    }
}

int AVLTree::isBalanced(const std::unique_ptr<Node>& node) const {
    if (!node) {
        return 0;
    }
    return static_cast<int>(getHeight(node->leftTree) - getHeight(node->rightTree));
}

std::unique_ptr<AVLTree::Node> AVLTree::rotateLL(std::unique_ptr<Node> y) {
    std::unique_ptr<Node> x = std::move(y->leftTree);
    y->leftTree = std::move(x->rightTree);
    x->rightTree = std::move(y);
    updateHeight(x->rightTree);
    updateHeight(x);
    return x;
}

std::unique_ptr<AVLTree::Node> AVLTree::rotateRR(std::unique_ptr<Node> x) {
    std::unique_ptr<Node> y = std::move(x->rightTree);
    x->rightTree = std::move(y->leftTree);
    y->leftTree = std::move(x);
    updateHeight(y->leftTree);
    updateHeight(y);
    return y;
}

std::unique_ptr<AVLTree::Node> AVLTree::rotateLR(std::unique_ptr<Node> node) {
    node->leftTree = rotateRR(std::move(node->leftTree));
    return rotateLL(std::move(node));
}

std::unique_ptr<AVLTree::Node> AVLTree::rotateRL(std::unique_ptr<Node> node) {
    node->rightTree = rotateLL(std::move(node->rightTree));
    return rotateRR(std::move(node));
}

std::unique_ptr<AVLTree::Node> AVLTree::balancing(std::unique_ptr<Node> node) {
    int balance = isBalanced(node);
    if (balance > 1) {
        if (getHeight(node->leftTree->leftTree) >= getHeight(node->leftTree->rightTree)) {
            return rotateLL(std::move(node));
        }
        return rotateLR(std::move(node));
    }
    if (balance < -1) {
        if (getHeight(node->rightTree->rightTree) >= getHeight(node->rightTree->leftTree)) {
            return rotateRR(std::move(node));
        }
        return rotateRL(std::move(node));
    }
    updateHeight(node);
    return node;
}

void AVLTree::copyRecord(Record& dst, const Record& src) {
    dst.number = src.number;
    dst.name = src.name;
}

std::unique_ptr<AVLTree::Node>* AVLTree::findLeftMax(std::unique_ptr<Node>& node) {
    if (node->rightTree) {
        std::unique_ptr<Node>* res = findLeftMax(node->rightTree);
        node = balancing(std::move(node));
        return res;
    }
    node->height--;
    return &node;
}

std::unique_ptr<AVLTree::Node>* AVLTree::findRightMin(std::unique_ptr<Node>& node) {
    if (node->leftTree) {
        std::unique_ptr<Node>* res = findRightMin(node->leftTree);
        node = balancing(std::move(node));
        return res;
    }
    node->height--;
    return &node;
}

bool AVLTree::insertion(std::unique_ptr<Node>& current, std::unique_ptr<Record> currentRecord) {
    if (!current) {
        current = std::make_unique<Node>();
        current->record = std::move(currentRecord);
        current->height = 1;
        return true;
    }

    const std::uint64_t key = currentRecord->number.digits();
    const std::uint64_t cur = current->record->number.digits();

    if (key > cur) {
        if (!insertion(current->rightTree, std::move(currentRecord))) {
            return false;
        }
    } else if (key < cur) {
        if (!insertion(current->leftTree, std::move(currentRecord))) {
            return false;
        }
    } else {
        return false;
    }

    current = balancing(std::move(current));
    return true;
}

std::string AVLTree::searchName(const Node* current, std::uint64_t key) const {
    if (!current) {
        return "not_found";
    }
    if (key > current->record->number.digits()) {
        return searchName(current->rightTree.get(), key);
    }
    if (key < current->record->number.digits()) {
        return searchName(current->leftTree.get(), key);
    }
    return current->record->name;
}

bool AVLTree::deletion(std::unique_ptr<Node>& current, const Record& key) {
    if (!current) {
        return false;
    }

    const std::uint64_t k = key.number.digits();
    const std::uint64_t cur = current->record->number.digits();

    if (k > cur) {
        if (!deletion(current->rightTree, key)) {
            return false;
        }
    } else if (k < cur) {
        if (!deletion(current->leftTree, key)) {
            return false;
        }
    } else {
        if (!current->leftTree && !current->rightTree) {
            current.reset();
        } else {
            std::unique_ptr<Node>* swapNodePtr = nullptr;
            if (getHeight(current->leftTree) >= getHeight(current->rightTree)) {
                swapNodePtr = findLeftMax(current->leftTree);
            } else {
                swapNodePtr = findRightMin(current->rightTree);
            }
            copyRecord(*current->record, *(*swapNodePtr)->record);
            if (!deletion(*swapNodePtr, *(*swapNodePtr)->record)) {
                return false;
            }
        }
    }

    if (current) {
        current = balancing(std::move(current));
    }
    return true;
}

void AVLTree::printInfix(const Node* node, std::string& cursor) const {
    if (!node) {
        cursor += "(null)";
        return;
    }

    cursor += "(";
    if (node->leftTree || node->rightTree) {
        printInfix(node->leftTree.get(), cursor);
        cursor += ",";
    }

    const char* prefix = (node->record->number.prefix() == '7') ? "+7" : "8";
    char buf[64];
    std::snprintf(buf, sizeof(buf), "\"%s%010llu\"", prefix,
                  static_cast<unsigned long long>(node->record->number.digits()));
    cursor += buf;

    if (node->leftTree || node->rightTree) {
        cursor += ",";
        printInfix(node->rightTree.get(), cursor);
    }
    cursor += ")";
}

AVLTree::AVLTree() = default;

AVLTree::~AVLTree() = default;

bool AVLTree::insert(std::unique_ptr<Record> record) {
    return insertion(root_, std::move(record));
}

std::string AVLTree::findName(std::uint64_t key) const {
    return searchName(root_.get(), key);
}

bool AVLTree::remove(const Record& key) {
    return deletion(root_, key);
}

std::string AVLTree::formatTree() const {
    std::string buffer;
    printInfix(root_.get(), buffer);
    return buffer;
}

bool AVLTree::empty() const {
    return !root_;
}

AVLTree::Node::Node() : height(1) {}

AVLTree::Node::~Node() = default;
