#ifndef TREE_NODE_H
#define TREE_NODE_H

template <typename T>
class treeNode
{
    template <typename U>
    friend class binaryTree;
    template <typename U>
    friend class tree_iterator;
    template<typename U>
    friend class reverse_tree_iterator;

private:
    T data;
    treeNode* left;
    treeNode* right;
    treeNode* parent;  // Добавляем указатель на родителя

public:
    treeNode(const T& value)
        : data(value), left(nullptr), right(nullptr), parent(nullptr) {}

    T getData() const { return data; }
};

#endif // TREE_NODE_H
