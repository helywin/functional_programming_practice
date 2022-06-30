/*********************************************************************************
 * FileName: action.cpp
 * Author: helywin(jiang770882022@hotmail.com)
 * Version: 0.1
 * Date: 22-4-22
 * Description:
 * Others:
 *********************************************************************************/

#include <range/v3/action.hpp>
#include <algorithm>
#include <iostream>

using namespace ranges;

std::vector<std::string> read_text()
{
    return {"aaa", "00", "bbb"};
}

int main()
{
    std::vector<std::string> words = read_text()
                                     | action::sort
                                     | action::unique;
    std::for_each(words.begin(), words.end(), [](const std::string &s) {
        std::cout << s << " ";
    });
}