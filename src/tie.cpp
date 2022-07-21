//
// Created by helywin on 2022/7/21.
//

#include <tuple>
#include <string>
#include <iostream>

struct person_t
{
    std::string m_name;
    std::string m_surname;
};

bool operator<(const person_t &left, const person_t &right)
{
    return std::tie(left.m_surname, left.m_name) <
           std::tie(right.m_surname, right.m_name);
}

int main()
{
    person_t m{"Mike", "Lee"};
    person_t t{"Taylor", "Swift"};
    person_t t1{"Mika", "Lef"};
    person_t t2{"Mikg", "Lea"};
    std::cout << (m < t) << std::endl;
    std::cout << (m < t1) << std::endl;
    std::cout << (m < t2) << std::endl;
}