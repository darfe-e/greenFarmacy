#ifndef TREE_ALGORITHMS_H
#define TREE_ALGORITHMS_H

#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
class tree_algorithms
{
public:
    using value_type = T;                                                 // Тип значений для удобства использования

    template <typename ForwardIterator>                                   // Вывод элементов в порядке возрастания
    void print_sorted_ascending(ForwardIterator begin, ForwardIterator end);

    template <typename ReverseIterator>                                   // Вывод элементов в порядке убывания
    void print_sorted_descending(ReverseIterator rbegin, ReverseIterator rend);

    template <typename ForwardIterator>                                   // Получение элементов в порядке возрастания
    std::vector<value_type> get_sorted_ascending(ForwardIterator begin, ForwardIterator end);

    template <typename ReverseIterator>                                   // Получение элементов в порядке убывания
    std::vector<value_type> get_sorted_descending(ReverseIterator rbegin, ReverseIterator rend);
};

template <typename T>
template <typename ForwardIterator>
void tree_algorithms<T>::print_sorted_ascending(ForwardIterator begin, ForwardIterator end)
{
    for (auto it = begin; it != end; ++it)                                // Проход от начала до конца
        std::cout << *it;                                                 // Вывод каждого элемента
}

template <typename T>
template <typename ReverseIterator>
void tree_algorithms<T>::print_sorted_descending(ReverseIterator rbegin, ReverseIterator rend)
{
    for (auto rit = rbegin; rit != rend; ++rit)                           // Обратный проход от rbegin до rend
        std::cout << *rit;                                                // Вывод каждого элемента
}

template <typename T>
template <typename ForwardIterator>
std::vector<typename tree_algorithms<T>::value_type>
tree_algorithms<T>::get_sorted_ascending(ForwardIterator begin, ForwardIterator end)
{
    std::vector<value_type> result;                                       // Вектор для хранения результата
    for (auto it = begin; it != end; ++it)                                // Проход от начала до конца
        result.push_back(*it);                                            // Добавление элемента в вектор
    return result;                                                        // Возврат отсортированного вектора
}

template <typename T>
template <typename ReverseIterator>
std::vector<typename tree_algorithms<T>::value_type>
tree_algorithms<T>::get_sorted_descending(ReverseIterator rbegin, ReverseIterator rend)
{
    std::vector<value_type> result;                                       // Вектор для хранения результата
    for (auto rit = rbegin; rit != rend; ++rit)                           // Обратный проход от rbegin до rend
        result.push_back(*rit);                                            // Добавление элемента в вектор
    return result;                                                        // Возврат отсортированного в обратном порядке вектора
}

#endif // TREE_ALGORITHMS_H
