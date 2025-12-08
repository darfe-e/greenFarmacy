#ifndef REVERSE_TREE_ITERATOR_H
#define REVERSE_TREE_ITERATOR_H

#include "treenode.h"
#include <stdexcept>

template <typename T>
class reverse_tree_iterator
{
private:
    treeNode<T>* current;                                                 // Текущий узел, на который указывает итератор
    treeNode<T>* tree_root;                                               // Корень дерева (для навигации)

public:
    reverse_tree_iterator();                                              // Конструктор по умолчанию
    reverse_tree_iterator(treeNode<T>* node, treeNode<T>* root = nullptr); // Конструктор с параметрами
    reverse_tree_iterator(const reverse_tree_iterator& other);            // Конструктор копирования

    reverse_tree_iterator& operator=(const reverse_tree_iterator& other); // Оператор присваивания

    T& operator*() const;                                                 // Оператор разыменования
    T* operator->() const;                                                // Оператор доступа к членам

    // В обратном итераторе ++ двигается к меньшему элементу
    reverse_tree_iterator& operator++();                                  // Префиксный инкремент
    reverse_tree_iterator operator++(int);                                // Постфиксный инкремент

    // В обратном итераторе -- двигается к большему элементу
    reverse_tree_iterator& operator--();                                  // Префиксный декремент
    reverse_tree_iterator operator--(int);                                // Постфиксный декремент

    bool operator==(const reverse_tree_iterator& other) const;            // Оператор сравнения на равенство
    bool operator!=(const reverse_tree_iterator& other) const;            // Оператор сравнения на неравенство

    treeNode<T>* getNode() const;                                         // Получение текущего узла
};

template <typename T>
reverse_tree_iterator<T>::reverse_tree_iterator()
    : current(nullptr), tree_root(nullptr)                                // Инициализация нулевыми указателями
{
}

template <typename T>
reverse_tree_iterator<T>::reverse_tree_iterator(treeNode<T>* node, treeNode<T>* root)
    : current(node), tree_root(root)                                      // Инициализация указателями
{
}

template <typename T>
reverse_tree_iterator<T>::reverse_tree_iterator(const reverse_tree_iterator& other)
    : current(other.current), tree_root(other.tree_root)                  // Копирование указателей
{
}

template <typename T>
reverse_tree_iterator<T>& reverse_tree_iterator<T>::operator=(const reverse_tree_iterator& other)
{
    if (this != &other)                                                   // Проверка самоприсваивания
    {
        current = other.current;                                          // Копирование текущего узла
        tree_root = other.tree_root;                                      // Копирование корня дерева
    }
    return *this;                                                         // Возврат текущего итератора
}

template <typename T>
T& reverse_tree_iterator<T>::operator*() const
{
    if (current == nullptr)                                               // Проверка нулевого указателя
        throw std::runtime_error("Dereferencing null iterator");
    return current->data;                                                 // Возврат данных текущего узла
}

template <typename T>
T* reverse_tree_iterator<T>::operator->() const
{
    if (current == nullptr)                                               // Проверка нулевого указателя
        throw std::runtime_error("Dereferencing null iterator");
    return &(current->data);                                              // Возврат указателя на данные узла
}

template <typename T>
reverse_tree_iterator<T>& reverse_tree_iterator<T>::operator++()
{
    // Для обратного итератора ++ двигается к предыдущему (меньшему) элементу
    if (current == nullptr)                                               // Если итератор указывает на конец (rend)
    {
        // Для rend() нужно найти минимальный элемент дерева
        if (tree_root == nullptr)                                         // Если дерево пустое
            return *this;                                                 // Возврат без изменений

        // Поиск минимального элемента (самый левый узел)
        current = tree_root;                                              // Начинаем с корня
        while (current->left != nullptr)                                  // Пока есть левый потомок
            current = current->left;                                      // Двигаемся влево
        return *this;                                                     // Возврат итератора на минимальный элемент
    }

    // Логика как у обычного итератора для оператора --
    if (current->left != nullptr)                                         // Если есть левый потомок
    {
        current = current->left;                                          // Переход к левому потомку
        while (current->right != nullptr)                                 // Поиск максимального в левом поддереве
            current = current->right;                                     // Двигаемся вправо
    }
    else                                                                  // Если нет левого потомка
    {
        treeNode<T>* parent = current->parent;                            // Переход к родителю
        while (parent != nullptr && current == parent->left)              // Подъем пока текущий - левый потомок
        {
            current = parent;                                             // Переход к родителю
            parent = parent->parent;                                      // Подъем дальше
        }
        current = parent;                                                 // Установка нового текущего узла
    }
    return *this;                                                         // Возврат обновленного итератора
}

template <typename T>
reverse_tree_iterator<T> reverse_tree_iterator<T>::operator++(int)
{
    reverse_tree_iterator temp = *this;                                   // Сохранение текущего значения
    ++(*this);                                                            // Инкремент
    return temp;                                                          // Возврат старого значения
}

template <typename T>
reverse_tree_iterator<T>& reverse_tree_iterator<T>::operator--()
{
    // Для обратного итератора -- двигается к следующему (большему) элементу
    if (current == nullptr)                                               // Если итератор указывает на начало (rbegin)
    {
        // Для rbegin() нужно найти максимальный элемент дерева
        if (tree_root == nullptr)                                         // Если дерево пустое
            return *this;                                                 // Возврат без изменений

        // Поиск максимального элемента (самый правый узел)
        current = tree_root;                                              // Начинаем с корня
        while (current->right != nullptr)                                 // Пока есть правый потомок
            current = current->right;                                     // Двигаемся вправо
        return *this;                                                     // Возврат итератора на максимальный элемент
    }

    // Логика как у обычного итератора для оператора ++
    if (current->right != nullptr)                                        // Если есть правый потомок
    {
        current = current->right;                                         // Переход к правому потомку
        while (current->left != nullptr)                                  // Поиск минимального в правом поддереве
            current = current->left;                                      // Двигаемся влево
    }
    else                                                                  // Если нет правого потомка
    {
        treeNode<T>* parent = current->parent;                            // Переход к родителю
        while (parent != nullptr && current == parent->right)             // Подъем пока текущий - правый потомок
        {
            current = parent;                                             // Переход к родителю
            parent = parent->parent;                                      // Подъем дальше
        }
        current = parent;                                                 // Установка нового текущего узла
    }
    return *this;                                                         // Возврат обновленного итератора
}

template <typename T>
reverse_tree_iterator<T> reverse_tree_iterator<T>::operator--(int)
{
    reverse_tree_iterator temp = *this;                                   // Сохранение текущего значения
    --(*this);                                                            // Декремент
    return temp;                                                          // Возврат старого значения
}

template <typename T>
bool reverse_tree_iterator<T>::operator==(const reverse_tree_iterator& other) const
{
    return current == other.current;                                      // Сравнение указателей на узлы
}

template <typename T>
bool reverse_tree_iterator<T>::operator!=(const reverse_tree_iterator& other) const
{
    return !(*this == other);                                             // Отрицание оператора равенства
}

template <typename T>
treeNode<T>* reverse_tree_iterator<T>::getNode() const
{
    return current;                                                       // Возврат текущего узла
}

#endif // REVERSE_TREE_ITERATOR_H
