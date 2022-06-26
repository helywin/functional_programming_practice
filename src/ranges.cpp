/*********************************************************************************
 * FileName: ranges.cpp
 * Author: helywin(jiang770882022@hotmail.com)
 * Version: 0.1
 * Date: 2022/4/20
 * Description:
 * Others:
*********************************************************************************/

#include <ranges>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <format>
#include <sstream>
#include <range/v3/action.hpp>

void func1()
{
    auto const ints = {0, 1, 2, 3, 4, 5};
    auto even = [](int i) { return 0 == i % 2; };
    auto square = [](int i) { return i * i; };

    // "pipe" syntax of composing the views:
    for (int i: ints | std::views::filter(even) | std::views::transform(square)) {
        std::cout << i << ' ';
    }

    std::cout << '\n';

    // a traditional "functional" composing syntax:
    for (int i: std::views::transform(std::views::filter(ints, even), square)) {
        std::cout << i << ' ';
    }
}

void func2()
{
    std::istringstream str("1 3 5 7 8 9 10");
    double sum = std::accumulate(std::istream_iterator<double>(str),
                                 std::istream_iterator<double>(),
                                 0);
    std::cout << std::format("total: {}", sum) << std::endl;
}

int main()
{

    func2();
//    std::vector<std::string> texts{"a", "dad", "action", "code", "physic"};
//    auto words = texts | ranges::v3::action::sort
//                       | ranges::v3::action::unique;
//    std::cout << words;
}

template<typename T>
bool operator==(const std::istream_iterator<T> &left,
                const std::istream_iterator<T> &right)
{
    if (left.is_sentinel() && right.is_sentinel()) { return true; }
    else if (left.is_sentinel()) {
        // Test whether sentinel predicate is
        // true for the right iterator
    } else if (right.is_sentinel()) {
        // Test whether sentinel predicate is
        // true for the left iterator
    } else {
        // Both iterators are normal iterators,
        // test whether they are pointing to the
        // same location in the collection
    }
}