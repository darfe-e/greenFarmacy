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
    treeNode<T>* root;                                                        // Корень бинарного дерева
    std::function<bool(const T&, const T&)> comparator;                       // Компаратор для сравнения элементов

    // Внутренний класс итератора для дерева
    class tree_iterator
    {
    private:
        treeNode<T>* current;                                                 // Текущий узел
        treeNode<T>* tree_root;                                               // Корень дерева (для навигации)

    public:
        tree_iterator() : current(nullptr), tree_root(nullptr) {}             // Конструктор по умолчанию
        tree_iterator(treeNode<T>* node, treeNode<T>* root = nullptr)         // Конструктор с параметрами
            : current(node), tree_root(root) {}
        tree_iterator(const tree_iterator& other)                             // Конструктор копирования
            : current(other.current), tree_root(other.tree_root) {}

        tree_iterator& operator=(const tree_iterator& other)                  // Оператор присваивания
        {
            if (this != &other)                                               // Проверка самоприсваивания
            {
                current = other.current;                                      // Копирование текущего узла
                tree_root = other.tree_root;                                  // Копирование корня
            }
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
            if (current == nullptr) return *this;                             // Если конец - возврат без изменений

            if (current->right != nullptr)                                    // Если есть правый потомок
            {
                current = current->right;                                     // Переход к правому потомку
                while (current->left != nullptr)                              // Поиск минимального в правом поддереве
                    current = current->left;
            }
            else                                                              // Если нет правого потомка
            {
                treeNode<T>* parent = current->parent;                        // Переход к родителю
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

        bool operator==(const tree_iterator& other) const                     // Оператор сравнения на равенство
        {
            return current == other.current;                                  // Сравнение указателей на узлы
        }

        bool operator!=(const tree_iterator& other) const                     // Оператор сравнения на неравенство
        {
            return !(*this == other);                                         // Отрицание оператора равенства
        }

        treeNode<T>* getNode() const { return current; }                      // Получение текущего узла
    };

public:
    using iterator = tree_iterator;                                           // Псевдоним для итератора

    binaryTree();                                                             // Конструктор по умолчанию
    binaryTree(std::function<bool(const T&, const T&)> comp);                 // Конструктор с компаратором
    ~binaryTree();                                                            // Деструктор

    iterator begin();                                                         // Итератор на начало (неконстантный)
    iterator end();                                                           // Итератор на конец (неконстантный)
    iterator begin() const;                                                   // Итератор на начало (константный)
    iterator end() const;                                                     // Итератор на конец (константный)

    void push(const T& value);                                                // Добавление элемента
    bool remove(const T& value);                                              // Удаление элемента
    void clear();                                                             // Очистка дерева
    bool empty() const;                                                       // Проверка пустоты дерева
    size_t size() const;                                                      // Получение размера дерева

    template<typename Predicate>                                              // Поиск по условию (неконстантный)
    iterator find_if(Predicate pred);
    template<typename Predicate>                                              // Поиск по условию (константный)
    iterator find_if(Predicate pred) const;

    void printTree() const;                                                   // Вывод дерева на экран

private:
    treeNode<T>* insert(treeNode<T>* node, treeNode<T>* parent, const T& value); // Рекурсивное добавление
    treeNode<T>* findMinNode(treeNode<T>* node) const;                        // Поиск минимального узла
    void clearRecursive(treeNode<T>* node);                                   // Рекурсивная очистка
    size_t sizeRecursive(treeNode<T>* node) const;                            // Рекурсивный подсчет размера

    template<typename Predicate>                                              // Рекурсивный поиск по условию
    treeNode<T>* findIfRecursive(treeNode<T>* node, Predicate pred) const;

    void printRecursive(treeNode<T>* node, int depth = 0) const;              // Рекурсивный вывод
};

template <typename T>
binaryTree<T>::binaryTree() : root(nullptr),
    comparator([](const T& a, const T& b) { return a < b; })                  // Компаратор по умолчанию (оператор <)
{
}

template <typename T>
binaryTree<T>::binaryTree(std::function<bool(const T&, const T&)> comp)
    : root(nullptr), comparator(comp)                                         // Инициализация компаратора
{
}

template <typename T>
binaryTree<T>::~binaryTree()
{
    clear();                                                                  // Очистка дерева при уничтожении
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::begin()
{
    treeNode<T>* leftmost = root;                                             // Начало с корня
    if (leftmost != nullptr)                                                  // Если дерево не пустое
    {
        while (leftmost->left != nullptr)                                     // Поиск самого левого узла
            leftmost = leftmost->left;
    }
    return iterator(leftmost, root);                                          // Возврат итератора на начало
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::end()
{
    return iterator(nullptr, root);                                           // Итератор на конец (nullptr)
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::begin() const
{
    treeNode<T>* leftmost = root;                                             // Начало с корня (константный)
    if (leftmost != nullptr)                                                  // Если дерево не пустое
    {
        while (leftmost->left != nullptr)                                     // Поиск самого левого узла
            leftmost = leftmost->left;
    }
    return iterator(leftmost, root);                                          // Возврат итератора на начало
}

template <typename T>
typename binaryTree<T>::iterator binaryTree<T>::end() const
{
    return iterator(nullptr, root);                                           // Итератор на конец (nullptr)
}

template <typename T>
void binaryTree<T>::push(const T& value)
{
    if (root == nullptr)                                                      // Если дерево пустое
    {
        root = new treeNode<T>(value);                                        // Создание корневого узла
    }
    else                                                                      // Если дерево не пустое
    {
        root = insert(root, nullptr, value);                                  // Рекурсивное добавление
    }
}

template <typename T>
treeNode<T>* binaryTree<T>::insert(treeNode<T>* node, treeNode<T>* parent, const T& value)
{
    if (node == nullptr)                                                      // Если достигли места для вставки
    {
        treeNode<T>* newNode = new treeNode<T>(value);                        // Создание нового узла
        newNode->parent = parent;                                             // Установка родителя
        return newNode;                                                       // Возврат нового узла
    }

    if (comparator(value, node->data))                                        // Если значение меньше текущего
    {
        node->left = insert(node->left, node, value);                         // Рекурсивный вызов для левого поддерева
    }
    else if (comparator(node->data, value))                                   // Если значение больше текущего
    {
        node->right = insert(node->right, node, value);                       // Рекурсивный вызов для правого поддерева
    }

    return node;                                                              // Возврат текущего узла
}

template <typename T>
bool binaryTree<T>::remove(const T& value)
{
    auto it = find_if([&value](const T& item) {                               // Поиск элемента для удаления
        return !(item < value) && !(value < item);                            // Проверка равенства
    });

    if (it == end()) return false;                                            // Если элемент не найден

    treeNode<T>* node = it.getNode();                                         // Получение узла для удаления
    if (node == nullptr) return false;                                        // Проверка нулевого указателя

    if (node->left == nullptr && node->right == nullptr)                      // Случай 1: узел без потомков
    {
        if (node->parent != nullptr)                                          // Если узел не корневой
        {
            if (node->parent->left == node)                                   // Если узел - левый потомок
                node->parent->left = nullptr;                                 // Обнуление указателя у родителя
            else                                                              // Если узел - правый потомок
                node->parent->right = nullptr;                                // Обнуление указателя у родителя
        }
        else                                                                  // Если узел корневой
        {
            root = nullptr;                                                   // Обнуление корня
        }
        delete node;                                                          // Удаление узла
    }
    else if (node->left == nullptr || node->right == nullptr)                 // Случай 2: узел с одним потомком
    {
        treeNode<T>* child = (node->left != nullptr) ? node->left : node->right;  // Определение единственного потомка
        child->parent = node->parent;                                         // Установка родителя у потомка

        if (node->parent != nullptr)                                          // Если узел не корневой
        {
            if (node->parent->left == node)                                   // Если узел - левый потомок
                node->parent->left = child;                                   // Замена на потомка
            else                                                              // Если узел - правый потомок
                node->parent->right = child;                                  // Замена на потомка
        }
        else                                                                  // Если узел корневой
        {
            root = child;                                                     // Установка потомка как корня
        }
        delete node;                                                          // Удаление узла
    }
    else                                                                      // Случай 3: узел с двумя потомками
    {
        treeNode<T>* successor = findMinNode(node->right);                    // Поиск преемника (мин в правом поддереве)
        node->data = successor->data;                                         // Копирование данных преемника

        if (successor->parent->left == successor)                             // Если преемник - левый потомок
            successor->parent->left = successor->right;                       // Замена на правого потомка преемника
        else                                                                  // Если преемник - правый потомок
            successor->parent->right = successor->right;                      // Замена на правого потомка преемника

        if (successor->right != nullptr)                                      // Если у преемника есть правый потомок
            successor->right->parent = successor->parent;                     // Обновление родителя

        delete successor;                                                     // Удаление преемника
    }

    return true;                                                              // Удаление успешно
}

template <typename T>
treeNode<T>* binaryTree<T>::findMinNode(treeNode<T>* node) const
{
    while (node != nullptr && node->left != nullptr)                          // Пока есть левый потомок
        node = node->left;                                                    // Движение влево
    return node;                                                              // Возврат минимального узла
}

template <typename T>
void binaryTree<T>::clear()
{
    clearRecursive(root);                                                     // Рекурсивная очистка
    root = nullptr;                                                           // Обнуление корня
}

template <typename T>
void binaryTree<T>::clearRecursive(treeNode<T>* node)
{
    if (node == nullptr) return;                                              // Базовый случай рекурсии

    clearRecursive(node->left);                                               // Очистка левого поддерева
    clearRecursive(node->right);                                              // Очистка правого поддерева
    delete node;                                                              // Удаление текущего узла
}

template <typename T>
bool binaryTree<T>::empty() const
{
    return root == nullptr;                                                   // Проверка пустого корня
}

template <typename T>
size_t binaryTree<T>::size() const
{
    return sizeRecursive(root);                                               // Рекурсивный подсчет размера
}

template <typename T>
size_t binaryTree<T>::sizeRecursive(treeNode<T>* node) const
{
    if (node == nullptr) return 0;                                            // Базовый случай рекурсии
    return 1 + sizeRecursive(node->left) + sizeRecursive(node->right);        // 1 + размер левого + размер правого
}

template <typename T>
template<typename Predicate>
typename binaryTree<T>::iterator binaryTree<T>::find_if(Predicate pred)
{
    treeNode<T>* found = findIfRecursive(root, pred);                         // Рекурсивный поиск
    return iterator(found, root);                                             // Возврат итератора
}

template <typename T>
template<typename Predicate>
typename binaryTree<T>::iterator binaryTree<T>::find_if(Predicate pred) const
{
    treeNode<T>* found = findIfRecursive(root, pred);                         // Рекурсивный поиск (константный)
    return iterator(found, root);                                             // Возврат итератора
}

template <typename T>
template<typename Predicate>
treeNode<T>* binaryTree<T>::findIfRecursive(treeNode<T>* node, Predicate pred) const
{
    if (node == nullptr) return nullptr;                                      // Базовый случай рекурсии

    if (pred(node->data)) return node;                                        // Если условие выполнено

    treeNode<T>* leftResult = findIfRecursive(node->left, pred);              // Поиск в левом поддереве
    if (leftResult != nullptr) return leftResult;                             // Если найден в левом поддереве

    return findIfRecursive(node->right, pred);                                // Поиск в правом поддереве
}

template <typename T>
void binaryTree<T>::printTree() const
{
    printRecursive(root);                                                     // Рекурсивный вывод
    std::cout << std::endl;                                                   // Перевод строки
}

template <typename T>
void binaryTree<T>::printRecursive(treeNode<T>* node, int depth) const
{
    if (node == nullptr) return;                                              // Базовый случай рекурсии

    printRecursive(node->right, depth + 1);                                   // Вывод правого поддерева

    for (int i = 0; i < depth; ++i)                                           // Отступы для визуализации уровня
        std::cout << "   ";
    std::cout << node->data << std::endl;                                     // Вывод данных узла

    printRecursive(node->left, depth + 1);                                    // Вывод левого поддерева
}
#endif // BINARY_TREE_H
