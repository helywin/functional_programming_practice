#include <iostream>
#include <vector>
#include <fstream>
#include "prettyprint.hpp"

std::vector<int>
count_lines_in_files(const std::vector<std::string> &files)
{
    std::vector<int> results;
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

int main()
{
    std::vector<std::string> files{
            "/usr/include/c++/7/iostream",
            "/usr/include/c++/7/fstream",
            "/usr/include/c++/7/vector",
    };
    std::cout << "lines: " << count_lines_in_files(files) << std::endl;
    return 0;
}
