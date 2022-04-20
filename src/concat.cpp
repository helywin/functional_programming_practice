/*********************************************************************************
 * FileName: concat.cpp
 * Author: helywin(jiang770882022@hotmail.com)
 * Version: 0.1
 * Date: 2022/4/20
 * Description: 
 * Others:
*********************************************************************************/

#include <string>
#include <iostream>

template<typename... Strings>
class lazy_string_concat_helper;

template<typename LastString, typename... Strings>
class lazy_string_concat_helper<LastString, Strings...>
{
private:
    // 存储原来字符串的拷贝
    const LastString &data;
    lazy_string_concat_helper<Strings...> tail;

public:
    lazy_string_concat_helper(const LastString &data, lazy_string_concat_helper<Strings...> tail) :
            data(data), tail(tail)
    {}

    int size() const
    {
        return data.size() + tail.size();
    }

    template<typename It>
    void save(It end) const
    {
        const auto begin = end - data.size();
        std::copy(data.cbegin(), data.cend(), begin);
        tail.save(begin);
    }

    explicit operator std::string() const
    {
        std::string result(size(), '\0');
        save(result.end());
        return result;
    }

    explicit operator int() const
    {
        return size();
    }

    lazy_string_concat_helper<std::string, LastString, Strings...>
    operator+(const std::string &other) const
    {
        return lazy_string_concat_helper<std::string, LastString, Strings...>(other, *this);
    }
};

template<>
class lazy_string_concat_helper<>
{
public:
    lazy_string_concat_helper()
    {}

    int size() const
    {
        return 0;
    }

    template<typename It>
    void save(It) const
    {}

    lazy_string_concat_helper<std::string> operator+(const std::string &other) const
    {
        return {other, *this};
    }
};

lazy_string_concat_helper<> lazy_concat;

int main()
{
    std::string name = "Jane";
    std::string surname = "Smith";
    const auto fullname =
            lazy_concat + surname + std::string(", ") + name;
//    name = "John";
    std::cout << "begin print" << std::endl;
    auto result = (std::string)fullname;
    std::cout << result << std::endl;
}