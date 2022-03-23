#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
//#include <ranges>
#include "prettyprint.hpp"

// 最常用的办法
std::vector<int> count_lines_in_files1(const std::vector<std::string> &files)
{
    std::vector<int> results;
    results.reserve(files.size());
    char c = 0;
    for (const auto &file: files) {
        int line_count = 0;
        std::ifstream in(file);
        while (in.get(c)) {
            if (c == '\n') {
                line_count++;
            }
        }
        results.push_back(line_count);
    }
    return results;
}

// 用函数模板数
int count_lines(const std::string &filename)
{
    std::ifstream in(filename);
    return (int) std::count(std::istreambuf_iterator<char>(in),
                            std::istreambuf_iterator<char>(),
                            '\n');
}

std::vector<int> count_lines_in_files2(const std::vector<std::string> &files)
{
    std::vector<int> results;
    results.reserve(files.size());
    for (const auto &file: files) {
        results.push_back(count_lines(file));
    }
    return results;
}

// 用函数模板数进行转换
std::vector<int> count_lines_in_files3(const std::vector<std::string> &files)
{
    std::vector<int> results(files.size());
    std::transform(files.cbegin(), files.cend(),
                   results.begin(),
                   count_lines);
    return results;
}


// 报错
//std::vector<int> count_lines_in_files4(const std::vector<std::string> &files)
//{
//    std::vector<int> result;
//    result = files | std::views::transform(count_lines);
//    return result;
//}

int main()
{
    std::vector<std::string> files{
            "/usr/include/c++/7/iostream",
            "/usr/include/c++/7/fstream",
            "/usr/include/c++/7/vector",
    };
    std::cout << "lines: " << count_lines_in_files1(files) << std::endl;
    std::cout << "lines: " << count_lines_in_files2(files) << std::endl;
    std::cout << "lines: " << count_lines_in_files3(files) << std::endl;
    return 0;
}
