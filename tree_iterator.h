#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include "treenode.h"
#include <stdexcept>

template <typename T>
class tree_iterator
{
private:
    treeNode<typename std::remove_const<T>::type>* current;

public:
    // Конструкторы
    tree_iterator() : current(nullptr) {}

    // Принимает указатель на treeNode (неконстантный или константный)
    tree_iterator(treeNode<typename std::remove_const<T>::type>* node) : current(node) {}

    tree_iterator(const tree_iterator& other) : current(other.current) {}

    // Конструктор для конвертации из iterator в const_iterator
    template<typename U>
    tree_iterator(const tree_iterator<U>& other,
                  typename std::enable_if<std::is_convertible<U, T>::value>::type* = nullptr)
        : current(other.getNode()) {}

    tree_iterator& operator=(const tree_iterator& other)
    {
        if (this != &other)
        {
            current = other.current;
        }
        return *this;
    }

    T& operator*() const
    {
        if (current == nullptr)
            throw std::runtime_error("Dereferencing null iterator");
        return current->data;
    }

    T* operator->() const
    {
        if (current == nullptr)
            throw std::runtime_error("Dereferencing null iterator");
        return &(current->data);
    }

    tree_iterator& operator++()
    {
        if (current == nullptr)
            throw std::runtime_error("Cannot increment end iterator");

        if (current->right != nullptr)
        {
            current = current->right;
            while (current->left != nullptr)
                current = current->left;
        }
        else
        {
            treeNode<typename std::remove_const<T>::type>* parent = current->parent;
            while (parent != nullptr && current == parent->right)
            {
                current = parent;
                parent = parent->parent;
            }
            current = parent;
        }
        return *this;
    }

    tree_iterator operator++(int)
    {
        tree_iterator temp = *this;
        ++(*this);
        return temp;
    }

    tree_iterator& operator--()
    {
        if (current == nullptr)
            throw std::runtime_error("Cannot decrement end iterator");

        if (current->left != nullptr)
        {
            current = current->left;
            while (current->right != nullptr)
                current = current->right;
        }
        else
        {
            treeNode<typename std::remove_const<T>::type>* parent = current->parent;
            while (parent != nullptr && current == parent->left)
            {
                current = parent;
                parent = parent->parent;
            }
            current = parent;
        }
        return *this;
    }

    tree_iterator operator--(int)
    {
        tree_iterator temp = *this;
        --(*this);
        return temp;
    }

    bool operator==(const tree_iterator& other) const
    {
        return current == other.current;
    }

    bool operator!=(const tree_iterator& other) const
    {
        return !(*this == other);
    }

    treeNode<typename std::remove_const<T>::type>* getNode() const
    {
        return current;
    }
};

#endif // TREE_ITERATOR_H
