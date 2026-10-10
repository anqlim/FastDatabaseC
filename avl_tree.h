#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "record.h"

class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    bool insert(std::unique_ptr<Record> record);
    std::string findName(std::uint64_t key) const;
    bool remove(const Record& key);
    std::string formatTree() const;
    bool empty() const;

private:
    struct Node {
        Node();
        ~Node();

        std::unique_ptr<Record> record;
        std::unique_ptr<Node> leftTree;
        std::unique_ptr<Node> rightTree;
        size_t height;
    };

    std::unique_ptr<Node> root_;

    size_t getHeight(const std::unique_ptr<Node>& node) const;
    void updateHeight(const std::unique_ptr<Node>& node);
    int isBalanced(const std::unique_ptr<Node>& node) const;
    std::unique_ptr<Node> rotateLL(std::unique_ptr<Node> y);
    std::unique_ptr<Node> rotateRR(std::unique_ptr<Node> x);
    std::unique_ptr<Node> rotateLR(std::unique_ptr<Node> node);
    std::unique_ptr<Node> rotateRL(std::unique_ptr<Node> node);
    std::unique_ptr<Node> balancing(std::unique_ptr<Node> node);
    static void copyRecord(Record& dst, const Record& src);
    std::unique_ptr<Node>* findLeftMax(std::unique_ptr<Node>& node);
    std::unique_ptr<Node>* findRightMin(std::unique_ptr<Node>& node);
    bool insertion(std::unique_ptr<Node>& current, std::unique_ptr<Record> currentRecord);
    std::string searchName(const Node* current, std::uint64_t key) const;
    bool deletion(std::unique_ptr<Node>& current, const Record& key);
    void printInfix(const Node* node, std::string& cursor) const;
};
