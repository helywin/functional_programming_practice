//
// Created by helywin on 2022/8/7.
//

#include <exception>
#include <stdexcept>
#include <vector>
#include <optional>
#include <variant>
#include <iostream>

template<typename T, typename E>
class expected
{
private:
    union
    {
        T m_value;
        E m_error;
    };

    bool m_valid;
    // 默认构造， 只能自己调用
    explicit expected(bool valid = false) : m_valid(valid)
    {
    }

public:
    // 拷贝构造
    expected(const expected &other) :
            m_valid(other.m_valid)
    {
        if (m_valid) {
            // 拷贝new构造
            new(&m_value) T(other.m_value);
        } else {
            new(&m_error) E(other.m_error);
        }
    }

    // 移动构造
    expected(expected &&other) noexcept:
            m_valid(other.m_valid)
    {
        if (m_valid) {
            // 移动new构造
            new(&m_value) T(std::move(other.m_value));
        } else {
            new(&m_error) E(std::move(other.m_error));
        }
    }

    // 联合体必须指定析构类型
    ~expected()
    {
        if (m_valid) {
            m_value.~T();
        } else {
            m_error.~E();
        }
    };

    // 为了std::swap调用，实现swappable
    void swap(expected &other)
    {
        if (m_valid) {
            if (other.m_valid) {
                // 都有效，交换值
                std::swap(m_value, other.m_value);
            } else {
                // 当前有效，交换对象无效的情况
                auto temp = std::move(other.m_error);
                other.m_error.~E();
                new(&other.m_value) T(std::move(m_value));
                m_value.~T();
                new(&m_error) E(std::move(temp));
                std::swap(m_valid, other.m_valid);
            }
        } else {
            if (other.m_valid) {
                // 另一个有效，另一个调用交换函数
                if (other.m_valid) {
                    other.swap(*this);
                } else {
                    // 都无效，交换错误
                    std::swap(m_error, other.m_error);
                }
            }
        }
    }

    // 转换为bool类型操作符，用于条件判断
    explicit operator bool() const
    {
        return m_valid;
    }

    // 转换成为optional操作符，比如std::optional(...)
    explicit operator std::optional<T>() const
    {
        if (m_valid) {
            return m_value;
        } else {
            return std::nullopt;
        }
    }

    T &get()
    {
        if (!m_valid) {
            throw std::logic_error("Missing a value");
        }
        return m_value;
    }

    E &error()
    {
        if (m_valid) {
            throw std::logic_error("There is no error");
        }
        return m_error;
    }

    template<typename ...Args>
    static expected success(Args &&...params)
    {
        expected result;
        result.m_valid = true;
        new(&result.m_value)
                T(std::forward<Args>(params)...);
        return result;
    }

    template<typename ...Args>
    static expected failed(Args &&...params)
    {
        expected result;
        result.m_valid = false;
        new(&result.m_value)
                E(std::forward<Args>(params)...);
        return result;
    }
};

// 实现get_if函数
template<typename T, typename Variant,
        typename Expected = expected<T, std::string>>
Expected get_if(const Variant &variant)
{
    const T *ptr = std::get_if<T>(variant);
    if (ptr) { return Expected::success(*ptr); }
    else {
        return Expected::failed("Variant doesn't contain the desired type");
    }
}

template<typename ...Args>
void print_result(const std::string &name, expected<Args...> &v)
{
    if (v) {
        std::cout << name << " succeed" << std::endl;
        std::cout << "val: " << v.get() << std::endl;
    } else {
        std::cout << name << " error" << std::endl;
        std::cout << "err: " << v.error() << std::endl;
    }
};

int main()
{

    auto result = expected<double, std::string>::success(1.0);
    print_result("result", result);
    auto result1 = expected<double, std::string>::failed("error occurred");
    print_result("result1", result1);
    auto op_result = std::optional(result1);
    std::variant<int, double> a{1};
    auto get_result = ::get_if<double>(&a);
    print_result("get_result", get_result);
}