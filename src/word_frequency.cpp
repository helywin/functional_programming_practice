//
// Created by helywin on 2022/7/3.
//

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <range/v3/all.hpp>
#include <ranges>

using namespace ranges;

std::string string_to_lower(const std::string &s)
{
    return s | view::transform(tolower);
}

std::string string_only_alnum(const std::string &s)
{
    return s | view::filter(isalnum);
}

int main()
{
    // 大写转小写

    // 是否为数字和字母组成

    std::vector<std::string> words =
            istream_range<std::string>(std::cin)
            | view::transform(string_to_lower)
            | view::transform(string_only_alnum)
            | view::remove_if(&std::string::empty);
    for (auto &word: words) {
        std::cout << word << std::endl;
    }
}
