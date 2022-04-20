/*********************************************************************************
 * FileName: fib.cpp
 * Author: helywin(jiang770882022@hotmail.com)
 * Version: 0.1
 * Date: 2022/4/20
 * Description: 
 * Others:
*********************************************************************************/

#include <iostream>

class fib_cache
{
public:
    fib_cache() :
        m_previous{0},
        m_last{1},
        m_size{2}
    {}

    size_t size() const
    {
        return m_size;
    }

    unsigned int operator[](unsigned int n) const
    {
        return n == m_size - 1 ? m_last :
               n == m_size - 2 ? m_previous :
                               0;
    }

    void push_back(unsigned int value)
    {
        m_size++;
        m_previous = m_last;
        m_last = value;
    }

private:
    int m_previous;
    int m_last;
    size_t m_size;
};

fib_cache cache;

unsigned int fib(unsigned int n)
{
    if (cache.size() > n) {
        return cache[n];
    } else {
        const auto result = fib(n - 1) + fib(n - 2);
        cache.push_back(result);
        return result;
    }
}

int main()
{
    std::cout << fib(5);
}
