#ifndef TREE_ITERATOR_H
#define TREE_ITERATOR_H

#include "treenode.h"
#include <stdexcept>

template <typename T>
class tree_iterator
{
private:
    treeNode<typename std::remove_const<T>::type>* current;               // Указатель на текущий узел

public:
    tree_iterator() : current(nullptr) {}                                 // Конструктор по умолчанию

    tree_iterator(treeNode<typename std::remove_const<T>::type>* node)    // Конструктор с указателем на узел
        : current(node) {}

    tree_iterator(const tree_iterator& other) : current(other.current) {} // Конструктор копирования

    // Конструктор для конвертации из iterator в const_iterator
    template<typename U>
    tree_iterator(const tree_iterator<U>& other,
                  typename std::enable_if<std::is_convertible<U, T>::value>::type* = nullptr)
        : current(other.getNode()) {}                                     // Использование SFINAE для безопасной конвертации

    tree_iterator& operator=(const tree_iterator& other)                  // Оператор присваивания
    {
        if (this != &other)                                               // Проверка самоприсваивания
            current = other.current;                                      // Копирование указателя на узел
        return *this;                                                     // Возврат текущего итератора
    }

    T& operator*() const                                                  // Оператор разыменования
    {
        if (current == nullptr)                                           // Проверка нулевого указателя
            throw std::runtime_error("Dereferencing null iterator");
        return current->data;                                             // Возврат данных узла
    }

    T* operator->() const                                                 // Оператор доступа к членам
    {
        if (current == nullptr)                                           // Проверка нулевого указателя
            throw std::runtime_error("Dereferencing null iterator");
        return &(current->data);                                          // Возврат указателя на данные
    }

    tree_iterator& operator++()                                           // Префиксный инкремент
    {
        if (current == nullptr)                                           // Проверка конца итератора
            throw std::runtime_error("Cannot increment end iterator");

        if (current->right != nullptr)                                    // Если есть правый потомок
        {
            current = current->right;                                     // Переход к правому потомку
            while (current->left != nullptr)                              // Поиск минимального в правом поддереве
                current = current->left;
        }
        else                                                              // Если нет правого потомка
        {
            treeNode<typename std::remove_const<T>::type>* parent = current->parent;  // Получение родителя
            while (parent != nullptr && current == parent->right)         // Подъем пока текущий - правый потомок
            {
                current = parent;                                         // Переход к родителю
                parent = parent->parent;                                  // Подъем дальше
            }
            current = parent;                                             // Установка нового текущего узла
        }
        return *this;                                                     // Возврат обновленного итератора
    }

    tree_iterator operator++(int)                                         // Постфиксный инкремент
    {
        tree_iterator temp = *this;                                       // Сохранение текущего значения
        ++(*this);                                                        // Инкремент
        return temp;                                                      // Возврат старого значения
    }

    tree_iterator& operator--()                                           // Префиксный декремент
    {
        if (current == nullptr)                                           // Проверка конца итератора
            throw std::runtime_error("Cannot decrement end iterator");

        if (current->left != nullptr)                                     // Если есть левый потомок
        {
            current = current->left;                                      // Переход к левому потомку
            while (current->right != nullptr)                             // Поиск максимального в левом поддереве
                current = current->right;
        }
        else                                                              // Если нет левого потомка
        {
            treeNode<typename std::remove_const<T>::type>* parent = current->parent;  // Получение родителя
            while (parent != nullptr && current == parent->left)          // Подъем пока текущий - левый потомок
            {
                current = parent;                                         // Переход к родителю
                parent = parent->parent;                                  // Подъем дальше
            }
            current = parent;                                             // Установка нового текущего узла
        }
        return *this;                                                     // Возврат обновленного итератора
    }

    tree_iterator operator--(int)                                         // Постфиксный декремент
    {
        tree_iterator temp = *this;                                       // Сохранение текущего значения
        --(*this);                                                        // Декремент
        return temp;                                                      // Возврат старого значения
    }

    bool operator==(const tree_iterator& other) const                     // Оператор сравнения на равенство
    {
        return current == other.current;                                  // Сравнение указателей на узлы
    }

    bool operator!=(const tree_iterator& other) const                     // Оператор сравнения на неравенство
    {
        return !(*this == other);                                         // Отрицание оператора равенства
    }

    treeNode<typename std::remove_const<T>::type>* getNode() const        // Получение текущего узла
    {
        return current;                                                   // Возврат указателя на узел
    }
};

#endif // TREE_ITERATOR_H
