#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "treenode.h"
#include <functional>
#include <vector>
#include <iostream>
#include <stdexcept>

template <typename T>
class binaryTree
{
private:
    treeNode<T>* root;
    std::function<bool(const T&, const T&)> comparator;

    // Простой итератор для дерева
    class tree_iterator
    {
    private:
        treeNode<T>* current;
        treeNode<T>* tree_root;

    public:
        tree_iterator() : current(nullptr), tree_root(nullptr) {}
        tree_iterator(treeNode<T>* node, treeNode<T>* root = nullptr)
            : current(node), tree_root(root) {}
        tree_iterator(const tree_iterator& other)
            : current(other.current), tree_root(other.tree_root) {}

        tree_iterator& operator=(const tree_iterator& other) {
            if (this != &other) {
                current = other.current;
                tree_root = other.tree_root;
            }
            return *this;
        }

        T& operator*() const {
            if (current == nullptr)
                throw std::runtime_error("Dereferencing null iterator");
            return current->data;
        }

        T* operator->() const {
            if (current == nullptr)
                throw std::runtime_error("Dereferencing null iterator");
            return &(current->data);
        }

        tree_iterator& operator++() {
            if (current == nullptr) return *this;

            if (current->right != nullptr) {
                current = current->right;
                while (current->left != nullptr)
                    current = current->left;
            } else {
                treeNode<T>* parent = current->parent;
                while (parent != nullptr && current == parent->right) {
                    current = parent;
                    parent = parent->parent;
                }
                current = parent;
            }
            return *this;
        }

        tree_iterator operator++(int) {
            tree_iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const tree_iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const tree_iterator& other) const {
            return !(*this == other);
        }

        treeNode<T>* getNode() const { return current; }
    };

public:
    using iterator = tree_iterator;
    // Убираем const_iterator - используем только iterator

    binaryTree();
    binaryTree(std::function<bool(const T&, const T&)> comp);
    ~binaryTree();

    // Методы begin/end - только неконстантные
    iterator begin();
    iterator end();

    // Если нужны константные методы, используем const версии
    iterator begin() const;
    iterator end() const;

    void push(const T& value);
    bool remove(const T& value);
    void clear();
    bool empty() const;
    size_t size() const;

    // Метод find_if - только неконстантный
    template<typename Predicate>
    iterator find_if(Predicate pred);

    // Если нужен константный find_if, возвращаем iterator
    template<typename Predicate>
    iterator find_if(Predicate pred) const;

    void printTree() const;

private:
    treeNode<T>* insert(treeNode<T>* node, treeNode<T>* parent, const T& value);
    treeNode<T>* findMinNode(treeNode<T>* node) const;
    void clearRecursive(treeNode<T>* node);
    size_t sizeRecursive(treeNode<T>* node) const;

    template<typename Predicate>
    treeNode<T>* findIfRecursive(treeNode<T>* node, Predicate pred) const;

    void printRecursive(treeNode<T>* node, int depth = 0) const;
};


template <typename T>
binaryTree<T>::binaryTree() : root(nullptr),
    comparator([](const T& a, const T& b) { return a < b; }) {}

template <typename T>
binaryTree<T>::binaryTree(std::function<bool(const T&, const T&)> comp)
    : root(nullptr), comparator(comp) {}

template <typename T>
binaryTree<T>::~binaryTree()
{
    clear();
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::begin()
{
    treeNode<T>* leftmost = root;
    if (leftmost != nullptr)
    {
        while (leftmost->left != nullptr)
            leftmost = leftmost->left;
    }
    return iterator(leftmost, root);
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::end()
{
    return iterator(nullptr, root);
}

// Константные версии begin/end - возвращают обычные итераторы
template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::begin() const
{
    treeNode<T>* leftmost = root;
    if (leftmost != nullptr)
    {
        while (leftmost->left != nullptr)
            leftmost = leftmost->left;
    }
    return iterator(leftmost, root);
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::end() const
{
    return iterator(nullptr, root);
}

template <typename T>
void binaryTree<T>::push(const T& value)
{
    if (root == nullptr)
    {
        root = new treeNode<T>(value);
    }
    else
    {
        root = insert(root, nullptr, value);
    }
}

template <typename T>
treeNode<T>* binaryTree<T>::insert(treeNode<T>* node, treeNode<T>* parent, const T& value)
{
    if (node == nullptr)
    {
        treeNode<T>* newNode = new treeNode<T>(value);
        newNode->parent = parent;
        return newNode;
    }

    if (comparator(value, node->data))
    {
        node->left = insert(node->left, node, value);
    }
    else if (comparator(node->data, value))
    {
        node->right = insert(node->right, node, value);
    }

    return node;
}

template <typename T>
bool binaryTree<T>::remove(const T& value)
{
    auto it = find_if([&value](const T& item) {
        return !(item < value) && !(value < item);
    });

    if (it == end()) return false;

    treeNode<T>* node = it.getNode();
    if (node == nullptr) return false;

    if (node->left == nullptr && node->right == nullptr)
    {
        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
                node->parent->left = nullptr;
            else
                node->parent->right = nullptr;
        }
        else
        {
            root = nullptr;
        }
        delete node;
    }
    else if (node->left == nullptr || node->right == nullptr)
    {
        treeNode<T>* child = (node->left != nullptr) ? node->left : node->right;
        child->parent = node->parent;

        if (node->parent != nullptr)
        {
            if (node->parent->left == node)
                node->parent->left = child;
            else
                node->parent->right = child;
        }
        else
        {
            root = child;
        }
        delete node;
    }
    else
    {
        treeNode<T>* successor = findMinNode(node->right);
        node->data = successor->data;

        if (successor->parent->left == successor)
            successor->parent->left = successor->right;
        else
            successor->parent->right = successor->right;

        if (successor->right != nullptr)
            successor->right->parent = successor->parent;

        delete successor;
    }

    return true;
}

template <typename T>
treeNode<T>* binaryTree<T>::findMinNode(treeNode<T>* node) const
{
    while (node != nullptr && node->left != nullptr)
        node = node->left;
    return node;
}

template <typename T>
void binaryTree<T>::clear()
{
    clearRecursive(root);
    root = nullptr;
}

template <typename T>
void binaryTree<T>::clearRecursive(treeNode<T>* node)
{
    if (node == nullptr) return;

    clearRecursive(node->left);
    clearRecursive(node->right);
    delete node;
}

template <typename T>
bool binaryTree<T>::empty() const
{
    return root == nullptr;
}

template <typename T>
size_t binaryTree<T>::size() const
{
    return sizeRecursive(root);
}

template <typename T>
size_t binaryTree<T>::sizeRecursive(treeNode<T>* node) const
{
    if (node == nullptr) return 0;
    return 1 + sizeRecursive(node->left) + sizeRecursive(node->right);
}

template <typename T>
template<typename Predicate>
typename binaryTree<T>::iterator binaryTree<T>::find_if(Predicate pred)
{
    treeNode<T>* found = findIfRecursive(root, pred);
    return iterator(found, root);
}

// Константная версия find_if - возвращает обычный итератор
template <typename T>
template<typename Predicate>
typename binaryTree<T>::iterator binaryTree<T>::find_if(Predicate pred) const
{
    treeNode<T>* found = findIfRecursive(root, pred);
    return iterator(found, root);
}

template <typename T>
template<typename Predicate>
treeNode<T>* binaryTree<T>::findIfRecursive(treeNode<T>* node, Predicate pred) const
{
    if (node == nullptr) return nullptr;

    if (pred(node->data)) return node;

    treeNode<T>* leftResult = findIfRecursive(node->left, pred);
    if (leftResult != nullptr) return leftResult;

    return findIfRecursive(node->right, pred);
}

template <typename T>
void binaryTree<T>::printTree() const
{
    printRecursive(root);
    std::cout << std::endl;
}

template <typename T>
void binaryTree<T>::printRecursive(treeNode<T>* node, int depth) const
{
    if (node == nullptr) return;

    printRecursive(node->right, depth + 1);

    for (int i = 0; i < depth; ++i)
        std::cout << "   ";
    std::cout << node->data << std::endl;

    printRecursive(node->left, depth + 1);
}
#endif // BINARY_TREE_H
