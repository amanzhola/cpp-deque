#pragma once

#include <deque>
#include <algorithm>   // std::merge
#include <iterator>    // std::back_inserter
#include <functional> // std::ref (обертка для тестов)

template<class T, class Comp>
std::deque<T> Merge(const std::deque<T>& half1,
                    const std::deque<T>& half2,
                    const Comp& comparator)
{
    std::deque<T> result;
    // Стабильное слияние двух отсортированных диапазонов при помощи итераторов
    std::merge(half1.begin(), half1.end(),
               half2.begin(), half2.end(),
               std::back_inserter(result),
               std::ref(comparator)); // 🧪 для тестов
    return result;
}

template<class T, class Comp>
std::deque<T> MergeSort(const std::deque<T>& src,
                        const Comp& comparator)
{
    if (src.size() <= 1) {
        return src; // базовый случай
    }

    const auto mid_it = src.begin() + static_cast<std::ptrdiff_t>(src.size() / 2);

    // Рекурсивно сортируем левую и правую части (конструируя их из диапазонов)
    std::deque<T> left (src.begin(), mid_it);
    std::deque<T> right(mid_it,       src.end());

    left  = MergeSort(left,  comparator);
    right = MergeSort(right, comparator);

    // Сливаем отсортированные половины
    return Merge(left, right, comparator);
}
