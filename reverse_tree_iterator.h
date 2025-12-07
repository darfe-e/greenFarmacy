#ifndef REVERSE_TREE_ITERATOR_H
#define REVERSE_TREE_ITERATOR_H

#include "treenode.h"
#include <stdexcept>

template <typename T>
class reverse_tree_iterator
{
private:
    treeNode<T>* current;
    treeNode<T>* tree_root;

public:

    reverse_tree_iterator();
    reverse_tree_iterator(treeNode<T>* node, treeNode<T>* root = nullptr);
    reverse_tree_iterator(const reverse_tree_iterator& other);

    reverse_tree_iterator& operator=(const reverse_tree_iterator& other);

    T& operator*() const;
    T* operator->() const;

    // В reverse итераторе ++ идет к меньшему, -- к большему
    reverse_tree_iterator& operator++();
    reverse_tree_iterator operator++(int);

    reverse_tree_iterator& operator--();
    reverse_tree_iterator operator--(int);

    bool operator==(const reverse_tree_iterator& other) const;
    bool operator!=(const reverse_tree_iterator& other) const;

    treeNode<T>* getNode() const;
};

template <typename T>
reverse_tree_iterator<T>::reverse_tree_iterator()
    : current(nullptr), tree_root(nullptr) {}

template <typename T>
reverse_tree_iterator<T>::reverse_tree_iterator(treeNode<T>* node, treeNode<T>* root)
    : current(node), tree_root(root) {}

template <typename T>
reverse_tree_iterator<T>::reverse_tree_iterator(const reverse_tree_iterator& other)
    : current(other.current), tree_root(other.tree_root) {}

template <typename T>
reverse_tree_iterator<T>& reverse_tree_iterator<T>::operator=(const reverse_tree_iterator& other)
{
    if (this != &other)
    {
        current = other.current;
        tree_root = other.tree_root;
    }
    return *this;
}

template <typename T>
T& reverse_tree_iterator<T>::operator*() const
{
    if (current == nullptr)
        throw std::runtime_error("Dereferencing null iterator");
    return current->data;
}

template <typename T>
T* reverse_tree_iterator<T>::operator->() const
{
    if (current == nullptr)
        throw std::runtime_error("Dereferencing null iterator");
    return &(current->data);
}

template <typename T>
reverse_tree_iterator<T>& reverse_tree_iterator<T>::operator++()
{
    // ++ для reverse идет к предыдущему (меньшему) элементу
    if (current == nullptr)
    {
        // Для rend() нужно найти минимальный элемент
        if (tree_root == nullptr)
            return *this;

        // Идем к минимальному элементу
        current = tree_root;
        while (current->left != nullptr)
            current = current->left;
        return *this;
    }

    // Логика как у обычного итератора для --
    if (current->left != nullptr)
    {
        current = current->left;
        while (current->right != nullptr)
            current = current->right;
    }
    else
    {
        treeNode<T>* parent = current->parent;
        while (parent != nullptr && current == parent->left)
        {
            current = parent;
            parent = parent->parent;
        }
        current = parent;
    }
    return *this;
}

template <typename T>
reverse_tree_iterator<T> reverse_tree_iterator<T>::operator++(int)
{
    reverse_tree_iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename T>
reverse_tree_iterator<T>& reverse_tree_iterator<T>::operator--()
{
    // -- для reverse идет к следующему (большему) элементу
    if (current == nullptr)
    {
        // Для rbegin() нужно найти максимальный элемент
        if (tree_root == nullptr)
            return *this;

        // Идем к максимальному элементу
        current = tree_root;
        while (current->right != nullptr)
            current = current->right;
        return *this;
    }

    // Логика как у обычного итератора для ++
    if (current->right != nullptr)
    {
        current = current->right;
        while (current->left != nullptr)
            current = current->left;
    }
    else
    {
        treeNode<T>* parent = current->parent;
        while (parent != nullptr && current == parent->right)
        {
            current = parent;
            parent = parent->parent;
        }
        current = parent;
    }
    return *this;
}

template <typename T>
reverse_tree_iterator<T> reverse_tree_iterator<T>::operator--(int)
{
    reverse_tree_iterator temp = *this;
    --(*this);
    return temp;
}

template <typename T>
bool reverse_tree_iterator<T>::operator==(const reverse_tree_iterator& other) const
{
    return current == other.current;
}

template <typename T>
bool reverse_tree_iterator<T>::operator!=(const reverse_tree_iterator& other) const
{
    return !(*this == other);
}

template <typename T>
treeNode<T>* reverse_tree_iterator<T>::getNode() const
{
    return current;
}

#endif // REVERSE_TREE_ITERATOR_H
