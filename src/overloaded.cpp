
#include <variant>
#include <string>
#include <iostream>

template<typename ...Ts>
struct overloaded : Ts ...
{
    using Ts::operator()...;
};

template<typename ...Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct Point
{
    double x;
    double y;
};

int main()
{
    using V = std::variant<int, double, std::string, Point>;
    V v_int{1};
    V v_dbl{1.0};
    V v_str{"aa"};
    V v_pnt{Point{1.0, 2.0}};

    auto ov = overloaded{
            [](int i) {
                std::cout << "int:" << i << std::endl;
            },
            [](double i) {
                std::cout << "double:" << i << std::endl;
            },
            [](std::string i) {
                std::cout << "std::string:" << i << std::endl;
            },
            [](Point i) {
                std::cout << "Point: {" << i.x << "," << i.y << "}" << std::endl;
            },
    };

    std::visit(ov, v_int);
    std::visit(ov, v_dbl);
    std::visit(ov, v_str);
    std::visit(ov, v_pnt);
}