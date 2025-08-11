#pragma once

#include <deque>

template<class T, class Comp>
std::deque<T> Merge(const std::deque<T>& half1, const std::deque<T>& half2, const Comp& comparator) {
    std::deque<T> result;
    size_t i = 0, j = 0;

    // Слияние двух отсортированных частей
    while (i < half1.size() && j < half2.size()) {
        if (comparator(half1[i], half2[j])) {
            result.push_back(half1[i]);
            ++i;
        } else {
            result.push_back(half2[j]);
            ++j;
        }
    }

    // Добавляем оставшиеся элементы из первой половины
    while (i < half1.size()) {
        result.push_back(half1[i]);
        ++i;
    }

    // Добавляем оставшиеся элементы из второй половины
    while (j < half2.size()) {
        result.push_back(half2[j]);
        ++j;
    }

    return result;
}

template<class T, class Comp>
std::deque<T> MergeSort(const std::deque<T>& src, const Comp& comparator) {
    if (src.size() <= 1) {
        return src;
    }

    const size_t mid = src.size() / 2;

    std::deque<T> left_sorted(src.begin(), src.begin() + mid);
    std::deque<T> right_sorted(src.begin() + mid, src.end());

    // Рекурсивный вызов для сортировки левой и правой части
    left_sorted = MergeSort(left_sorted, comparator);
    right_sorted = MergeSort(right_sorted, comparator);

    // Слияние отсортированных частей
    return Merge(left_sorted, right_sorted, comparator);
}