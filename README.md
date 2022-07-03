---
title: C++函数式编程
date: 2022-3-30 14:49:23
updated: 2022-4-20 14:28:23
tags:
  - C++
excerpt: 《Functional Programming in C++》书中代码练习测试以及一些笔记
---

# 说明

《Functional Programming in C++》书中代码练习测试以及一些笔记，部分代码需要用到C++20可以使用在线编译器编译代码

地址：https://coliru.stacked-crooked.com/

或者自己编译gcc-11.2及以上版本安装

## 1 介绍

### 1.1 什么是函数式编程

- 用常用的函数范式模板代替一些循环等，比如`std::one_of()`,`std::count()`,`std::transform()`等
- 使用ranges（C++20）的管道操作`|`

### 1.2 纯函数

> 函数，只使用（但不修改）传递给它们的参数，以计算出结果

- OOP可以用更好的管理程序状态，但不能减少程序状态

例子，文本编辑器在按保存的时候用户输入了，该保存哪些？

- OOP通过封装活动部件使代码变得易懂。FP通过最大限度地减少活动部件来使代码易于理解。

比如在例子中计算多个文件的行数，如果计算时候文件在改写，那么就要考虑到不同的状态

这种写法可以规避掉`count_lines`的状态传递

```c++
std::vector<int> count_lines_in_files(const std::vector<std::string> &files)
{
    reuturn files | transform(count_lines);
}
```

避免状态改变逐层传递产生耦合！！

### 1.3 函数式思考

想着输入是什么，输出是什么，怎样把输入映射到输出，而不是算法的每一步去写代码

### 1.4 函数式编程的好处

#### 1.4.1 代码健壮和可读

函数式写法代码更加精炼

#### 1.4.2 并发和同步

共享改变状态是写并发时的问题。并发编程写纯函数，因为不改变状态，在单线程多线程一样使用

比如

```c++
std::vector<double> xs = {1, 2, 3, 4, ...};
auto result = sum(xs | transform(sqrt));
```

如果`sqrt`是纯函数，那么求和算法就可以切块累加然后合并了，不管是单线程还是多线程都可以运行

由于C++没有纯函数的标记，所以不能自动并行。如果是循环就没有那么轻易地实现并行了

> 注意，C++编译器在发现循环体为纯的时候会自动向量化或者做其他优化

#### 1.4.3 持续优化

## 2 开始函数式编程

### 2.1 函数把函数当参数

对于过滤人群的函数调用，可以总结为

```c++
filter: (collection<T>, (T → bool)) → collection<T>
```

而结构体的映射转换使用的`transform`原型为

```c++
transform: (collection<In>, (In → Out)) → collection<Out>
```

### 2.2 STL例子

#### 2.2.1 计算均值

通常做法

```c++
double average_score(const std::vector<int> &scores)
{
    int sum = 0;
    for (int score: scores) {
        sum += score;
    }
    return sum / (double) scores.size();
}
```

我们可以使用`std::accumulate`来实现：

```c++
double average_score(const std::vector<int> &scores)
{
    return std::accumulate(scores.cbegin(), scores.cend(), 0) /
           (double) scores.size();
}
```

并行版本的算法，需要使用`execution`里面的函数`std::reduce`

```c++
double average_score(const std::vector<int>& scores)
{
    return std::reduce(
            std::execution::par,
            scores.cbegin(), scores.cend(),
            0
    ) / (double) scores.size();
}
```

编译并行程序如果出现问题，例如

```
error: #error Intel(R) Threading Building Blocks 2018 is required; older versions are not supported.
```

需要编译最新版本的TBB库，教程在：<https://github.com/oneapi-src/oneTBB/blob/master/INSTALL.md>

#### 2.2.2 折叠

`std::accumulate`也可以自定义操作符，或者使用已经有的例如`std::multiplies`

`std::accumulate`是特殊的函数，会顺序执行，不能够进行并行化

<center>
<p><img src="https://s1.ax1x.com/2022/04/15/L8IinK.png"/></p>
</center>

使用`std::accumulate`计算行数

```c++
int f(int previous_count, char c)
{
    return (c != '\n') ? previous_count
                       : previous_count + 1;
}

int count_lines(const std::string & s)
{
    return std::accumulate(
        s.cbegin(), s.cend(),
        0,
        f
    );
}
```

该方法用`f`函数判断是否是回车符，自定义了`std::accumulate`的运算函数。如果需要从尾部运算到头部，把迭代器`cbegin`换成`crbegin`就可以了

#### 2.2.3 字符串修剪

需要用到的算法是`std::find_if`

```c++
bool is_not_space(char c)
{
    return c != ' ';
}

std::string trim_left(std::string s)
{
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), is_not_space));
    return s;
}

std::string trim_right(std::string s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(),
            s.end());
    return s;
}

std::string trim(std::string s)
{
    return trim_left(trim_right(std::move(s)));
}
```

使用`base()`函数把反向迭代器转换成正向迭代器

#### 2.2.4 集合划分

使用`std::partition`和`std::stable_partition`可以把集合按照特定要求一分为二排列

比如把女性放队列前面

```c++
std::partition(
    people.begin(), people.end(),
    is_female
);
```

而两者的区别就是`std::partition`排列的时候不会考虑相同类之间元素的顺序，效率相对后者较高。而`std::stable_partition`会保持同类元素之间的相对顺序

#### 2.2.5 过滤和变换

使用`std::remove_if`过滤

```c++
people.erase(
    std::remove_if(people.begin(), people.end(),
                   is_not_female),
    people.end());
```

`std::remove`或者`std::remove_if`并不能真正的删除元素，而是把要删除的元素移动到末尾，配合`std::erase`来彻底删除元素

使用`std::copy_if`拷贝特定元素

```c++
std::vector<person_t> females;

std::copy_if(people.cbegin(), people.cend(),
             std::back_inserter(females),
             is_female);
```

`std::back_inserter`可以在原有集合的基础上提供向后插入的容器，由于`std::copy_if`事先并不知道需要拷贝多少元素，所以需要动态扩充的容器来进行拷贝

`std::copy_if`四个参数的原型

```c++
template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last, 
                 OutputIt d_first, UnaryPredicate pred)
{
    while (first != last) {
        if (pred(*first))
            *d_first++ = *first;
        first++;
    }
    return d_first;
}
```

可以看到`std::copy_if`并不会扩充目标容器

用`std::copy_if`除了拷贝到`std::back_inserter`容器里面还可以拷贝到流里面，比如

```c++
std::ostream_iterator<int>(std::cout, " ")
```

### 2.3 STL算法的兼容性问题

通常STL算法的兼容性都比手写循环兼容性好，比如以下找出所有女性并把名字存储记录的代码

```c++
std::vector<std::string> names;

for (const auto &person : people) {
    if (is_female(person)) {
        names.push_back(name(person));
    }
}
```

使用`transform`加上`filter`函数可以简洁，原型如下

```c++
filter      : (collection<T>, (T -> bool)) -> collection<T>
transform   : (collection<T>, (T -> T2)) -> collection<T2>
```

`filter`是C++20里面的，在`<ranges>`头文件里面

```c++
std::vector<std::string> names;
transform(filter(people, is_female), name);
```

利用`filter`过滤出符合的元素然后进行一对一的集合元素转换

使用该方法解决了两个算法之间存在中介变量的问题，比如需要生命临时的用来存女性的集合的数组

使用两次`std::copy_if`也能达到`std::stable_partition`的功能

```c++
std::vector<person_t> separated(people.size());

const auto last = std::copy_if(
    people.cbegin(), people.cend(),
    separated.begin(),
    is_female);

std::copy_if(
    people.cbegin(), people.cend(),
    last,
    is_not_female);
```

### 2.4 编写自己的高等级函数

很多STL算法在第三方库里面也有实现，比如Boost

#### 2.4.1 以变量的方式接收函数

例如

```c++
template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t> &people,
    FilterFunction filter);
```

#### 2.4.2 使用循环实现

```c++
template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t> &people,
    FilterFunction filter)
{
    std::vector<std::string> result;

    for (const person_t &person : people) {
        if (filter(person)) {
            result.push_back(name(person));
        }
    }

    return result;
}
```

#### 2.4.3 递归和尾随调用优化

上面用循环的方式，每次找到一个人，都会相应的更改结果

使用递归方式如下：

```c++
template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t>& people,
    FilterFunction filter)
{
    if (people.empty()) {
        return {};
    } else {
        const auto head = people.front();
        const auto processed_tail = names_for(
                tail(people),
                filter);
        if (filter(head)) {
            return prepend(name(head), processed_tail);
        } else {
            return processed_tail;
        }
    }
}
```

该实现方式并不高效，`vector`并没有现成的`tail`函数，需要自己实现

另外一种实现方式如下：

```c++
template <typename FIlterFunction, typename Iterator>
std::vector<std::string> names_for(
    Iterator people_begin,
    Iterator people_end,
    FilterFunction filter)
{
    ...
    const auto processed_tail = names_for(
        people_begin + 1,
        people_end,
        filter);
    ...
}
```

第二种实现方法也会有每次都会在`vector`后面添加的情况，但是每次迭代占用的栈内存开销都很大，有些甚至会溢出崩溃

尾部递归的实现：

```c++
template <typename FilterFunction, typename Iterator>
std::vector<std::string> names_for_helper(
    Iterator people_begin,
    Iterator people_end,
    FilterFunction filter,
    std::vector<std::string> previously_collected)
{
    if (people_begin == people_end) {
        return previously_collected;
    }
    const auto head = *people_begin;
    
    if (filter(head)) {
        previously_collected.push_back(name(head));
    }

    return names_for_helper(
        people_begin + 1,
        people_end,
        filter,
        std::move(previously_collected));
}

// 调用函数
template <typename FilterFunction, typename Iterator>
std::vector<std::string> names_for_helper(
    Iterator people_begin,
    Iterator people_end,
    FilterFunction filter)
{
    names_for_helper(people_begin, people_end, filter, {});
}
```

#### 2.4.4 使用折叠实现

递归是一种低层次的结构，通常在春FP函数中会加以避免，前面说高层次的结构在FP里面更受欢迎，因为因为递归错综复杂

使用`std::accumulate`实现

```c++
std::vector<std::string> append_name_if(
    std::vector<std::string> previously_collected,
    const person_t &person)
{
    if (filter(person)) {
        previously_collected.push_back(name(person));
    }
    return previously_collected;
}

...

return std::accumulate(
    people.cbegin(),
    people.cend(),
    std::vector<std::string>{},
    append_name_if);
```

但是该例子里面有个重要的问题是拷贝次数太多了，每次调用`append_name_if`都会生成`previously_collected`的拷贝，C++20里面优化了`std::accumulate`的拷贝问题，也可以自己实现`move_accumulate`方法，以下是C++20优化后的实现

```c++
template<class InputIt, class T, class BinaryOperation>
constexpr // since C++20
T accumulate(InputIt first, InputIt last, T init, 
             BinaryOperation op)
{
    for (; first != last; ++first) {
        init = op(std::move(init), *first); // std::move since C++20
    }
    return init;
}
```

## 3 函数对象

### 3.1 函数和函数对象

两种不同的写法

```c++
int max(int arg1, int arg2) { ... }
auto max(int arg1, int arg2) -> int { ... }
```

#### 3.1.1 自动类型推导

```c++
int answer = 42;
auto ask1() { return answer; }
const auto &ask2() { return answer; }
```

使用`decltype(auto)`返回值

```c++
template <typename Object, typename Function>
decltype(auto) call_on_object(Object &&object, Function function)
{
    return function(std::forward<Object>(object));
}
```

**变量完美转发**

使用折叠引用和`std::forward`

#### 3.1.2 函数指针

```c++
int ask() { return 42; }

typedef decltype(ask) * function_ptr;

class convertible_to_function_ptr {
public:
    operator function_ptr() const   //函数指针callable_object
    {
        return ask;
    }
};

int main()
{
    auto ask_ptr = &ask;
    std::cout << ask_ptr() << '\n'; //调用函数指针

    auto ask_ref = ask;
    std::cout << ask_ref() << '\n'; //调用函数引用

    convertible_to_function_ptr ask_wrapper;
    std::cout << ask_wrapper() << '\n'; // 对象转换成函数指针
}

```

#### 3.1.3 重载调用操作符

语法：

```c++
class function_object {
public:
    return_type operator()(arguments) const
    {
        ...
    }
};
```

要取出年龄大于42岁的人，通常可以这么做

```c++
bool older_than_42(const person_t& person)
{
    return person.age > 42;
}
std::count_if(persons.cbegin(), persons.cend(),
              older_than_42);
```

但是这种方式并不通用，更加通用的办法是创建一个callable类，把42当构造参数传入

```c++
class older_than
{
public:
    older_than(int limit) : m_limit(limit) {}
    bool operator() (const person_t &person) const
    {
        return person.age() > m_limit;
    }

private:
    int m_limit;
}

older_than older_than_42(42);

std::count_if(persons.cbegin(), persons.cend(),
              older_than(42));
```

`std::count_if`并不需要传入参数为函数指针，只需要是能够被调用的就行

#### 3.1.4 创建通用函数对象

用于通用对象的比较函数对象

```c++
template<typename T>
class older_than {
public:
    older_than(int limit) :
            m_limit(limit) {}
    bool operator()(const T &object) const
    {
        return object.age() > m_limit;
    }
private:
    int m_limit;
};

std::count_if(persons.cbegin(), persons.cend(),
              older_than<person_t>(42));
std::count_if(cars.cbegin(), cars.cend(),
              older_than<car_t>(5));
std::count_if(projects.cbegin(), projects.cend(),
              older_than<project_t>(42));
```

上面函数对象以来类型来创建，通过只设置单个函数为模板函数创建能够自动推导类型的函数对象模板，使其更加通用

```c++
class older_than {
public:
    older_than(int limit) : m_limit(limit) {}
    template<typename T>
    bool operator()(T &&object) const
    {
        return std::forward<T(object).age() > m_limit;
    }
};

older_than predicate(5);
std::count_if(persons.cbegin(), persons.cend(), predicate);
std::count_if(cars.cbegin(), cars.cend(), predicate);
std::count_if(projects.cbegin(), projects.cend(), predicate);
```

### 3.2 Lambda表达式和闭包

使用$\lambda$表达式

```c++
std::copy_if(people.cbegin(), people.cend(),
             std::back_inserter(females),
             [](const person_t& person) {
                 return person.gender() == person_t::female;
             }
        );
```

#### 3.2.1 Lambda表达式语法

```c++
[a, &b] (int x, int y) { return a * x + b * y; }
   头         变量                函数体
```

不同函数头的作用

- `[a, &b]` —— a值抓取，b引用抓取
- `[]` —— 不使用附近作用域
- `[&]` —— 引用抓取所有函数体用过的变量
- `[=]` —— 值抓取所有函数体用过的变量
- `this` —— 抓取`this`指针
- `[&, a]` —— 除了`a`按值抓取，其他都按引用抓取
- `[=, &b]` —— 除了`b`按引用抓取，其他都按值抓取

#### 3.2.2 使用lambda表达式

```c++
class company_t {
public:
    std::string team_name_for(const person_t &employee) const;
    int count_team_members(const std::string &team_name) const;

private:
    std::vector<person_t> m_employees;
    ...
};

int company_t::count_team_members(const std::string &team_name) const
{
    return std::count_if(
            m_employees.cbegin(), m_employees.cend(),
            [this, &team_name](const person_t &employee) {
                return team_name_for(employee) == team_name;
            }
        );
}

```

使用`mutable`关键字标记的lambda表达式可以告诉编译器这个lambda上的调用操作符不应该是常数

```c++
int count = 0;
std::vector<std::string> words{"An", "ancient", "pond"};

```

> 当声明一个lambda表达式，编译器就会声明一个callable类，`()`操作符为函数

#### 3.2.3 在lambdas中创建任意的成员变量

使用值抓取move only类型`std::unique_ptr`时会有问题

```c++
std::unique_ptr<sesson_t> session = create_sesson();
auto request = server.request("GET /", session->id());
request.on_complete(
            [session]
            (response_t response)
            {
                std::cout << "Got response: " << response
                          << " for session: " << session;
            }
        );
```

上述方式会报错，需要在抓取的时候使用移动语义`std::move`

```c++
request.on_complete(
            [ session = std::move(session),
              time = current_time()
            ]
            (response_t response)
            {
                std::cout << "Got response: " << response
                          << " for session: " << session
                          << " the request took: "
                          << (current_time() - time)
                          << "milliseconds";
            }
        );
```

`time`在创建该匿名函数时就已经定义好了，无论后续在哪里调用

#### 3.2.4 通用lambda表达式

lambda表达式参数支持auto关键字，能够根据调用入参自己选择类型

```c++
auto predicate = [limit = 42] (auto &&object) {
    return object.age() > limit;
}

std::count_if(persons.cbegin(), persons.cend(),
              predicate);
std::count_if(cars.cbegin(), cars.cend(),
              predicate);
std::count_if(projects.cbegin(), projects.cend(),
              predicate);
```

C++20中更加通用的lambda表达式

支持使用decltype

```c++
[] (auto first, decltype(first) second) { … }
```

或者使用模板

```c++
[] <typename T> (T first, T second) { … }
```

### 3.3 编写比lambda表达式更复杂的函数对象

写过滤函数需要重复很多代码

```c++
ok_response = filter(response, 
        [] (const response_t &response) {
            return !response.error();
        });

failed_response = filter(response,
        [] (const response_t &response) {
            return response.error();
        });
```

理想的调用方式是这样的

```c++
ok_response = filter(response, not_error);
// 或者        filter(response, !error);
// 或者        filter(response, error == false);

failed_response = filter(response, error);
// 或者            filter(response, not_error == false);
// 或者            filter(response, error == true);
```

需要定义一个类，存储一个`bool`类型

```c++
class error_test_t {
public:
    error_test_t(bool error = true) : m_error(error) {}
    template <typename T>
    bool operator()(T &&value) const
    {
        return m_error == (bool) std::forward<T>(value).error();
    }

private:
    bool m_error;
};

error_test_t error(true);
error_test_t not_error(false);
```

如果需要支持`!`操作，还要重载该操作符

```c++
class error_test_t {
public:
    ...
    error_test_t operator==(bool test) const
    {
        return error_test_t(
            test ? m_error : !m_error
        );
    }

    error_test_t operator!() const
    {
        return error_test_t(!m_error);
    }
};
```

#### 3.3.1 STL中的操作符函数对象

STL中的操作符函数对象可以用在排序和累计中

```c++
std::vector<int> numbers{1, 2, 3, 4};
product = std::accumulate(numbers.cbegin(), numbers.cend(), 1,
                          std::multiplies<int>());
std::vector<int> numbers{5, 21, 13, 42};
std::sort(numbers.begin(), numbers.end(), std::greater<int>());
// numbers now contain {42, 21, 13, 5}
```

```c++
// 算术操作
std::plus
std::minus
std::multiplies
std::divides
std::modulus    // %
std::negates    // -a

// 比较操作
std::equal_to
std::not_equal_to
std::greater
std::less
std::greater_equal
std::less_equal

// 逻辑操作
std::logical_and
std::logical_or
std::logical_not

// 位操作
std::bit_and
std::bit_or
std::bit_xor
```

C++14中，在使用标准库中的运算符包装器时，你可以省略该类型。直接使用`std::greater<>()`，而不是`std::greater<int>()`

#### 3.3.2 其他库中的操作符函数对象

Boost库。比如Boost库中的`phoenix`

```c++
using namespace boost::phoenix::arg_names;
std::vector<int> numbers{21, 5, 62, 42, 53};
std::partition(numbers.begin(), numbers.end(),
               arg1 <= 42);
// numbers now contain {21, 5, 42, 62, 53}
//                         <= 42    > 42
```

`arg1`为定义的占位符，该函数代表按照第一个变量把集合划分开

然后还可以运用在其他上，比如累加

```c++
std::accumulate(numbers.cbegin(), numbers.cend(), 0, 
                arg1 + arg2 * arg2 / 2);
```

<center>
<p><img src="https://s1.ax1x.com/2022/04/15/L8Ik7D.png"/></p>
</center>

也可以算乘法或者进行排序

```c++
product = std::accumulate(numbers.cbegin(), numbers.cend(), 1,
                          arg1 * arg2);
std::sort(numbers.begin(), numbers.end(), arg1 > arg2);
```

### 3.4 使用std::function包装函数

```c++
std::function<float (float, float)> test_function;
test_function = std::fmaxf;                     // 函数
test_function = std::multiplies<float>();       // 类的调用函数
test_function = std::multiplies<>();            // 类通用调用函数
test_function = [x] (float a, float b) { return a * x + b; };   // lambda表达式
test_function = [x] (auto a, auto b) { return a * x + b; };     // 通用lanbda表达式
test_function = (arg1 + arg2) / 2;              // boost.phoenix 表达式
test_function = [](std::string s) { return s.empty(); } // 错误
```

`std::function`本质上是重载了`()`操作符的可调用类（callable）

`std::function`在绑定函数时需要用到`std::ref`确保是引用传参

```c++
std::function<void()> bound_f = std::bind(f, n1, std::ref(n2), std::cref(n3));
```

## 4 从旧的函数创建新的函数

### 4.1 部分功能的应用

之前的比较是用模板的形式，涉及到具体对象的方法，并不通用，下面采用的是重载调用操作符

```c++
class greater_than {
public:
    greater_than(int value) : m_value {}
    bool operator()(int arg) const
    {
        return arg > m_value;
    }
private:
    int m_value;
};
...

greater_than greater_than_42(42);
greater_than_42(1);     // false
greater_than_42(50);    // true

std::partition(xs.begin(), xs.end(), greater_than(6));
```

#### 4.1.1 将二元操作函数转换为单一函数的通用方法

更通用的方式封装别的用户的二元操作函数，比如说`>`是两个操作变量，greater_than把一个变量构造函数传入进去，变成一个一元函数

```c++
template<typename Function, typename SecondArgType>
class partial_application_on_2nd_impl {
public:
    partial_application_on_2nd_impl(Function function,
                                    SecondArgType second_arg)
        : m_function(function)
        , m_value(second_arg)
    {}

    template<typename FirstArgType>
    auto operator()(FirstArgType &&first_arg) const
        -> decltype(m_function(
                std::forward<FirstArgType>(first_arg),
                m_second_arg))
    {
        return m_function(
                std::forward<FirstArgType>(first_arg),
                m_second_arg);
    }

    ...
private:
    Function m_function;
    SecondArgType m_second_arg;
};

// 绑定函数
template<typename Function, typename SecondArgType>
partial_application_bind2nd_impl<Function, SecondArgType>
bind2nd(Function &&function, SecondArgType &&second_arg)
{
    return partial_application_bind2nd_impl<Function, SecondArgType>(
            std::forward<Function>(function),
            std::forward<SecondArgType>(second_arg));
}

auto greater_than_42 = bind2nd(std::greater<int>(), 42);

greater_than_42(1);     // false
greater_than_42(50);    // true
```

使用`bind2nd`把角度转换成弧度

```c++
// 把角度转换成弧度
std::vector<double> degrees = {0, 30, 45, 60};
std::vector<double> radians(degrees.size());

std::transform(degrees.cbegin(), degrees.cend(),
               radians.begin(),
               bind2nd(std::multiplies<double>, PI / 180));
```

#### 4.1.2 使用std::bind指定函数变量

`std::bind1st`和`std::bind2nd`都被弃用了,可以使用`std::bind`

```c++
auto bound = std::bind(std::greater<double>(), _1, 42);
bool is_6_greater_than_42 = bound(6);
```

占位符`_1`和`_2`在`<functional>`头文件的命名空间`std::placeholders`里面

通过绑定不同的占位符变为不同的操作

```c++
auto is_greater_than_42 =
    std::bind(std::greater<double>(), _1, 42);
auto is_less_than_42 =
    std::bind(std::greater<double>(), 42, _1);
```

#### 4.1.3 反转二元操作函数的变量

可以把`_1`和`_2`位置调换，使得变量传入也交换

```c++
std::sort(scores.begin(), scores.end(),
          std::bind(std::greater<double>, _2, _1));
```

#### 4.1.4 使用std::bind处理更多变量的函数

```c++
enum output_format_t
{
    name_only,
    full_name,
};

void print_person(const person_t &person,
                  std::ostream &out,
                  person_t::output_format_t format);

std::for_each(people.cbegin(), people.cend(),
        std::bind(print_person,
                  _1,
                  std::ref(std::cout),
                  person_t::name_only
            ));
```

也可以绑定类成员函数，比如`print`在`person_t`中

```c++
class person_t {
public:
    ...
    void print(std::ostream &out, person_t::output_format_t format);
}

std::for_each(people.cbegin(), people.cend(),
        std::bind(&person_t::print,
                 _1,
                 std::ref(std::cout), 
                 person_t::name_only
            ));
```

#### 4.1.5 使用lambda表达式替代std::bind

```c++
auto is_greater_than_42 = 
    [] (double value) {
        return std::greater<double>()(value, 42);
    };

auto is_less_than_42 = 
    [] (double value) {
        return std::greater<double>()(42, value);
    };
```

排序的实现

```c++
std::sort(scores.begin(), scores.end(),
          [] (double value1, double value2) {
              return std::greater<double>()(value2, value1);
          });
```

打印的实现

```c++
std::for_each(people.cbegin(), people.cend(),
              [] (const person_t &person) {
                  print_person(person,
                               std::cout,
                               person_t::name_only);
              });
std::for_each(people.cbegin(), people.cend(),
              [&file] (const person_t &person) {
                  print_person(person,
                               file,
                               person_t::name_only);
              });
```

### 4.2 Currying:不同的方式看函数

柯理化（currying）：从Haskell引进的词汇，把一个多参数函数转换成单参数函数, 并且返回一个能接受余下参数并返回结果的新函数。

例如，一个函数f0需要输出2个参数，Currying后，第一个函数f1接收第一个参数a1后返回一个函数f2，f2接受第二个参数a2后返回最终结果

```c++
// 正常版本的调用
int result = f0(10, 2);
 
// Currying版本的调用
auto f1 = f0(10);
int f2 = f1(2);
```

大于函数的Currying版本

```c++
// greater : (double, double) -> bool
bool greater(double first, double second)
{
    return first > second;
}

// greater_curried : double → (double → bool)
auto greater_curried(double first)
{
    return [first] (double second) {
        return first > second;
    }
}

greater(2, 3);          // false
greater_curried(2);     // 返回函数对象
greater_curried(2)(3);  // false
```

#### 4.2.1 更方便的创建Currying函数的方式

```c++
auto print_person_cd(const person_t &person)
{
    return [&] (std::ostream &out) {
        return [&] (person_t::output_format_t format) {
            print_person(person, out, format);
        }
    }
}
```

使用`make_curried`，后面第11章节会提供代码

```c++
using std::cout;

auto print_person_cd = make_curried(print_person);

// 所有的方式都可以调用
print_person_cd(martha, cout, person_t::full_name);
print_person_cd(martha)(cout, person_t::full_name);
print_person_cd(martha, cout)(person_t::full_name);
print_person_cd(martha)(cout)(person_t::full_name);

auto print_martha = print_person_cd(martha);
print_martha(cout, person_t::name_only);

auto print_martha_to_cout = print_person_cd(martha, cout);
print_martha_to_cout(person_t::name_only);
```

注意并不是所有的调用方式都可以，必须确保按照变量输入的先后顺序来进行

#### 4.2.2 使用Currying处理数据库权限

```c++
result_t query(connection_t &connection,
               session_t &session,
               const std::string &table_name,
               const std::string &filter);
```

考虑单一数据库，单一连接情况，创建一个query函数，使其能够满足以下API操作

```c++
auto table = "Movies";
auto filter = "Name = \"Sintel\"";
results = query(local_connection, session, table, filter);
auto local_query = query(local_connection);
auto remote_query = query(remote_connection);
results = local_query(session, table, filter);
auto main_query = query(local_connection, main_session);
results = main_query(table, filter);
auto movies_query = main_query(table);
results = movies_query(filter);
```

#### 4.2.3 Currying和部分函数应用

> 部分函数应用（Partial function application）：部分函数的应用是指以少于其总参数数的方式调用一个多参数函数。这将导致一个新的函数接受剩余的参数数。通过部分函数应用创建的简单函数是非常有用的，而且通常比匿名函数的语法要求要少得多^[1]。

部分函数应用可以改变绑定变量顺序，而Currying先绑定第一个变量，这个是区别

```c++
auto local_query = query(local_connection); 
auto local_query = std::bind(query, local_connection, _1, _2, _3); 
auto session_query = std::bind(query, _1, main_session, _2, _3); 
```

当你有一个特定的函数，你想绑定它的参数时，部分函数应用很有用。在这种情况下，你知道这个函数有多少个参数，你可以准确地选择你想绑定到一个特定的值的参数。当你可以得到一个有任意数量参数的函数时，Currying在一般情况下特别有用。因为在这种情况下，不知道变量个数意味着`std::bind`就不能实现。

### 4.3 函数构成

《编程珠玑》里面的“读取一个文本文件，确定最常使用的n个词，并打印出这些词的排序列表及其频率。”问题，Doug McIlroy的UNIX shell脚本解决方式：

```shell
tr -cs A-Za-z '\n' |
    tr A-Z a-z |
    sort |
    uniq -c |
    sort -rn |
    sed ${1}q
```

用C++来解决该问题的转换步骤：

1. 有一个文件。从里面把单词分出来
2. 把分出来的单词放到无序map里面`std::unordered_map<std::string, unsigned int>`，把每个单词的数目记录
3. 遍历map中的元素，把map转换成pair，并且数目在前
4. 排序
5. 打印

我们可以创建根据其他元素类型容器决定元素类型的容器

```c++
template <typename C,
          typename T = typename C::value_type>
std::unodered_map<T, unsigned int> count_occurrences(const C &collection);
```

最终看起来的形式

```c++
void print_common_words(const std::string& text)
{
    return print_pairs(
        sort_by_frequency(
            reverse_pairs(
                count_occurrences(
                    words(text)
                )
            )
        )
    );
}
```

### 4.4 再谈函数提升

提升（lifting）是一个编程术语，给你提供了一种方法，将一个给定的函数转化为一个适用于更广泛背景的类似函数。

> 提升是一个概念，它允许你将一个函数转化为另一个（通常是更一般的）环境中的相应函数。^[2]

拿字符串转大写举例

```c++
void to_upper(std::string &string);
```

实现方法比如：

```c++
void pointer_to_upper(std::string* str) 
{ 
    if (str) to_upper(*str); 
}

void vector_to_upper(std::vector<std::string>& strs) 
{ 
    for (auto& str : strs) { 
        to_upper(str); 
    } 
}

void map_to_upper(std::map<int, std::string>& strs) 
{ 
    for (auto& pair : strs) { 
        to_upper(pair.second); 
    } 
} 
```

这种实现方法看起来毫无价值，实现取决于容器类型，不得不去实现针对各种变量类型的方法

应该创建一个high-order函数，只关注于操作单一的字符串，和一个函数处理字符串指针，创建一个函数处理字符串向量和字符串map。这些函数称为提升函数，因为它们将对某一类型进行操作的函数提升到一个结构或包含该类型的集合。

使用C++14特性来实现

```c++
void to_upper(std::string &string);

// 指针提升
template <typename Function>
auto pointer_lift(Function f)
{
    return [f] (auto *item) {
        if (item) {
            f(*item);
        }
    }
}

// 集合提升
template <typename Function>
auto collection_lift(Function f)
{
    return [f] (auto &items) {
        for (auto &item : items) {
            f(item);
        }
    };
}

// 例子
std::string str{"lift"};
std::string *pstr = new std::string{"lift"};
std::vector<std::string> vstr{"lift", "function"};
to_upper(str);
pointer_lift(to_upper)(pstr);
collection_lift(to_upper)(vstr);
```

#### 4.4.1 颠倒成对的列表

在之前那个问题中，第3步，涉及需要把map转化成pair集合并且数目要提到前面去，代码如下

```c++
template <typename C,
          typename P1 = typename std::remove_cv<
                typename C::value_type::first_type>::type,
          typename P2 = typename C::value_type::second_type
          >
std::vector<std::pair<P2, P1>> reverse_pairs(const C &items)
{
    std::vector<std::pair<P2, P1>> result(item.size());
    std::transform(
        std::begin(items), std::end(items),
        std::begin(result),
        [] (const std::pair<const P1, P2> &p) {
            return std::make_pair(p.second, p.first);
        }
    );
    return result;
}
```

`std::remove_cv`可以移除`const`和`volatile`

## 5 纯粹:避免可变状态

### 5.1 可变状态的问题

例如电影类`movie_t`

```c++
// 电影类
class movie_t {
public:
    double average_score() const;
    ...

private:
    std::string name;
    std::list<int> scores;
};

// 计算平均评分的方法
double movie_t::average_score() const
{
    return std::accumulate(scores.begin(),
                           scores.end(), 0)
            / (double) scores.size();
}
```

问题出现在当你在计算平均分时别人在添加分数，新添加的可能在里面也可能不在里面。`.size()`函数可能返回的时之前的大小。也就是迭代器事先确定好了，但是计算的时候又加了元素，所以size会变大，平均值变小了。

C++11之前的`std::list`并不会保存列表大小，所以每次都会重新遍历计算。所以需要在电影类里面自己存储一个`size`

```c++
class movie_t {
public:
    double average_score() const;
    ...

private:
    std::string name;
    std::list<int> scores;
    size_t scores_size;
};
```

但是这样做的坏处时当项目越来越大，这种变量越来越多，重构的难度就越来越大。需要确保变量在使用的时候不可被修改。

### 5.2 纯粹的函数和指称的透明度

这些问题都来自一个设计缺陷：让软件系统中的几个组件负责相同的数据，而不知道另一个组件何时改变该数据。避免该问题出现的最简单的办法是禁止数据改动。

但是说起来简单做起来难。很多时候和用户交互面临这个问题。保存的时候用户在进行输入，一个按钮按下，其他按钮不知道用户鼠标点击了什么。

但是如果不提交改变任何状态，就不可能做任何事。只能计算你自己传入参数的程序。你不能
做交互程序，不能保存数据到磁盘或者数据库，不能发送网络请求等。程序就毫无用处。

首先还是理解纯和不纯函数。第1章说纯函数只使用传递给它们的参数值，以便返回结果。它们不需要有任何副作用来影响你程序中的任何其他函数或你系统中的任何其他程序。纯函数还需要在用相同的参数调用时总是返回相同的结果。

现在通过一个概念名为引用透明性(referential transparency)更精确的定义纯粹，引用透明性是表达式的一个特征，而不仅仅是函数。我们可以说表达式是任何指定计算并返回结果的东西。如果我们把整个表达式替换成它的返回值，程序的行为也不会有什么不同，我们就说表达式是引用透明性的。如果一个表达式是引用透明性的，那么它就没有可观察到的副作用，因此该表达式中使用的所有函数都是纯的。

> 参考透明度，是函数式编程中常用的一个术语，意味着给定一个函数和一个输入值，你将总是收到相同的输出。这就是说，在函数中没有使用外部状态。
> 也就是函数的输出只取决于他的输入^[3]

例如

```c++
// 全局变量G
int G = 10;

// 引用透明
int pulsOne(int x)
{
    return x + 1;
}

// 非引用透明
int plusG(int x)
{
    return x + G;
}
```

如果一个表达式是引用透明的，那么它就没有可观察的副作用（observable side effects），因此该表达式中使用的所有函数都是纯的。

搜索和打印最大值

```c++
double max(const std::vector<double> &numbers)
{
    assert(!numbers.empty());
    auto result = std::max_element(numbers.cbegin(),
                                   numbers.cend());
    std::cerr << "Maximum is: " << *result << std::endl;
    return *result;
}

int main()
{
    auto sum_max = 
        max({1}) + 
        max({1, 2}) +
        max({1, 2, 3});
    
    std::cout << sum_max << std::endl;
}
```

相比以下代码

```c++
int main()
{
    auto sum_max =
        1 +
        2 +
        3;
    std::cout << sum_max << std::endl;
}
```

前面的`max`函数也能得出最后一样的结果，但是在这个过程中有输出打印到`std::cerr`了，所以并不是纯粹的。

### 5.3 无副作用编程

在纯函数变成中，创建新的值而不是改变值。不是改变对象的值，而是从之前那个对象创建拷贝，这个拷贝中值已经改变为新的。在电影例子中，当你在计算电影分数时，你已经有电影分数列表，别人不能更改，他们只能创建一个新的列表，在这个列表中加入新的分数。

这种方法起初听起来是反直觉的；它似乎与我们通常认为的世界运作方式相反。但奇怪的是，它背后的想法几十年来一直是科幻小说的一部分，甚至被一些古希腊哲学家所讨论。有一个流行的概念，即我们生活在许多平行世界中的一个--每当我们做出一个选择，我们就会创造一个新的世界，在这个世界中我们做出了确切的选择，还有一些平行世界，我们在其中做出了不同的选择。

在编程时，我们通常不关心所有可能的世界；我们只关心一个。这就是为什么我们认为我们在改变一个单一的世界，而不是一直在创造新的世界并抛弃以前的世界。这是一个有趣的概念，不管这是否是世界的运作方式，它在开发软件时确实有其优点。

在迷宫中行走的例子

```c++
while (1) {
    - 绘制迷宫和玩家
    - 读取用户输入
    - 是否可以移动（按下方向箭头键）检查目的地单元是否是墙，不是墙则移动到目的地
    - 检查是否到达退出点，到达则显示消息，退出循环
}
```

首先，迷宫不会改变，玩家的状态会改变，迷宫可以用不可改变的类。

在移动的过程中，我们需要计算以下内容：

- 移动的方向
- 之前的位置，知道从哪里移动
- 是否能够移动到目的地

```c++
position_t next_position(
            direction_t direction,
            const position_t &previous_position,
            const maze_t &maze)
{
    const position_t desired_position{previous_position, direction};
    return maze.is_wall(desired_position) ? previous_postion
                                          : desired_position;
}

position_t::position_t(const position_t &original,
                       direction_t direction)
    : x { direction == Left  ? original.x - 1 : 
          direction == Right ? original.x + 1 :
                               original.x     }
    , y { direction == Up    ? original.y + 1 :
          direction == Down  ? original.y - 1 :
                               original.y     }
{}
```

最终的循环

```c++
void process_events(const maze_t &maze,
                    const position_t &current_position)
{
    if (maze.is_exit(current_position)) {
        return;
    }

    const direction_t direction = ...;
    draw_maze();
    draw_player(current_position, direction);
    const auto new_position = next_position(direction,
                                            current_position,
                                            maze);
    process_events(maze, new_position);
}

int main()
{
    const maze_t maze("maze.data");
    process_events(maze, maze.start_position());
}
```

更大的组件需要考虑使用moveable组件。你可以创建一个巨大的、包罗万象的世界结构，每当你需要改变其中的某些东西时，你就会重新创建。这将有很大的性能开销（即使你使用为函数式编程优化的数据结构，如第8章所述），并会大大增加你的软件的复杂性。

相反，你通常会有一些可变的状态，你认为总是要复制和传递是低效的，你会对你的函数进行建模，使它们返回关于世界上应该改变的语句，而不是总是返回世界的新副本。这种方法所带来的是系统的可变和纯粹部分之间的明确分离。（设计模式中的命令模式）

### 5.4 并发环境下的可变和不可变状态

可变的状态会导致问题，因为它允许共享责任（这甚至违背了最佳的OOP实践）。这些问题在并发环境中得到了提升，因为责任可以由多个组件同时分担。

```c++
void client_disconnected(const client_t &client)
{
    // 释放客户端资源
    ...
    // 减少连接数目
    connected_clients--;
    if (connected_clients == 0) {
        // 省电模式
    }
}
```

并行运行的时候`connected_clients--`会同时执行，比如初值为2，两个都是`2-1=1`，最终结果变为1。简单的解决办法：让编程人员通过`mutexes`来禁用数据的并发。

> 我经常开玩笑说，与其捡起Djikstra的可爱缩写（mutex--代表互斥），不如把基本的同步对象称为 "瓶颈"。瓶颈有时是有用的，有时是不可缺少的，但它们从来不是好事。在最好的情况下，它们是一种必要的邪恶。任何鼓励任何人过度使用瓶颈、将瓶颈保持过久的东西都是不好的。这里的问题不仅仅是互斥锁和解锁中额外递归逻辑的直线性能影响，而是对整个应用程序并发性的更大、更广泛、更难以描述的影响。
>
> — David Butenhof on comp.programming.threads

锁有时候很有用，但是需要少用，而不是作为不良软件设计的借口。在`for`循环中使用锁，是低级结构，对于实现并发编程的高级抽象很有用，但不应该在正常的代码中出现太多。

阿姆达尔定律^[4]：

假设原来在一个系统中执行一个程序需要时间$T_{old}$，其中某一个部分占的时间百分比为$\alpha$，然后，把这一部分的性能提升$k$倍。即这一部分原来需要的时间为$\alpha T_{old}$，现在需要的时间变为$(\alpha T_{old}/k)$。则整个系统执行此程序需要的时间变为：
$$
S=\frac{1}{(1-\alpha)+\alpha/k}
$$

<center>
<p><img src="https://s1.ax1x.com/2022/04/15/L8IF0O.png"/></p>
</center>

解决问题的办法一个就是不使用并发，另外一个就是不使用改变的数据。而第三个办法就是使用改变数据但是不共享。

你有四个选择：拥有不可变的数据而不共享，拥有可变的数据而不共享，拥有不可变的数据而共享，以及拥有可变的数据而共享。只有最后一种情况才会有问题。

### 5.5 变为常量的重要性

C++有两种办法限制改变：`const`和`constexpr`关键字

```c++
const std::string name{"John Smith"};
std::string name_copy            = name;    // 拷贝
std::string &name_ref            = name;    // 可改变引用，错误
const std::string &name_constref = name;    // 不可改变引用
std::string *name_ptr            = &name;   // 可改变指针，错误    
const std::string *name_constptr = &name;   // 不可改变指针
```

`const`指针和`const`引用对象只能调用对象的`const`方法

```c++
class person_t
{
public:
    std::string name() { return m_name; }
    std::string name_const() const { return m_name; }

private:
    std::string m_name;
};

person_t person;
const auto &p = person;
std::string name = p.name();       // 错误
std::string name = p.name_const(); // 正确
```

#### 5.5.1 逻辑和内部常数

创建不可改变的C++类的最简单方法就是声明所有成员变量为常量

```c++
class person_t
{
public:
    const std::string name;
    const std::string surname;
    ...
};
```

或者使用getter函数封装

```c++
class person_t
{
public:
    std::string name() const;
    std::string surname() const;

private:
    std::string m_name;
    std::string m_surname;
    ...
};
```

使用`mutable`修饰成员变量使得`const`函数可以改变类成员变量

```c++
class person_t
{
public:
    employment_history_t employment_history() const
    {
        std::uniqu_lock<std::mutex>
            lock{m_employment_history_mutex};
        
        if (!m_employment_history.loaded()) {
            load_employment_history();
        }

        return m_employment_history;
    }
};
```

#### 5.5.2 优化临时成员函数

当你将类设计为不可变时，每当您想要创建setter成员函数时，你都需要创建一个函数，该函数返回一个对象的副本，在该副本中，特定的成员值被更改。

```c++
person_t new_person {
    old_person.with_name("Jonne")
              .with_surname("Jones);
};
```

`with_name`方法返回一个全新的`person_t`实例

实现：

```c++
class person_t
{
public:
    person_t with_name(const std::string &name) const &
    {
        person_t result(*this);
        result.m_name = name;
        return result;
    }

    person_t with_name(const std::string &name) &&
    {
        person_t result(std::move(*this));
        result.m_name = name;
        return result;
    }
};
```

在函数后面使用`const &`表示该函数只能用在`const`引用对象上,而`&&`代表该函数只能用在右值对象上

#### 5.5.3 const的陷阱

你可能使用过多的`const`而掉进了陷阱

**CONST禁止移动对象**

```c++
person_t some_function()
{
    person_t result;
    // do something
    return result;
}
...
person_t person = some_function();
```

如果编译器不进行优化,返回`person_t`时会创建一个新的实例给调用者。但是编译器会进行优化，称为命名返回值优化（named
return value optimization, NRVO）

`const`函数会打破这一规则，返回`const`对象会导致副本

**浅CONST**

另一个问题是，CONST很容易被颠覆，例如

```c++
class company_t
{
public:
    std::vector<std::string> employees_names() const;

private:
    std::vector<person_t *> m_employees;
};
```

你不能通过`employees_names`修改员工名称，但是类中实际的`m_employees`是以指针的形式存在的，这意味着你不可以改变指针的值，但是指针指向的数据是可以修改的

**propagate_const包装器**

C++17之后的`propagete_const`可以解决这个问题，在`<experimental/propagate_const>`里面

## 6 懒惰评估

NOTE: lazy evaluation，计算表达式的评估（evaluation）也就是计算结果，lazy的意思可以参考lazy load（懒加载）

计算需要花费时间，当两个矩阵A和B相乘，只需要写

```c++
auto P = A * B;
```

问题就是你可能不需要该计算结果，而浪费了CPU时间

另外一种办法就是说有需要时，`P`应该被计算为`A *
B`的结果。仅仅只是定义它，而不是立即做计算。当程序某个部分需要`P`的值，才来进行计算。不是在之前计算

可以通过lambda表达式来定义

```c++
auto P = [A, B] {
    return A * B;
}
```

如果有人需要值，则可以调用`P()`

但是带来的问题就是如果不止一次需要求值呢？这样就会每次都进行计算，所以在计算后记住值就很重要

这就是懒惰的表现：你不提前做工作，而是尽可能地推迟工作。因为你很懒，你也想避免多次做同样的事情，所以在你得到结果后，你会记住它。

### 6.1 C++的懒惰

不幸的是不像其他语言一样，C++不支持懒惰评估，但是提供了一些工具可以仿真程序行为。

`lazy_val`模板

```c++
template<typename F>
class lazy_val
{
private:
    F m_computation;
    mutable bool m_cache_initialized;
    mutable decltype(m_computation()) m_cache;
    mutable std::mutex m_cache_mutex;

public:
    lazy_val(F computation) :
            m_computation(computation),
            m_cache_initialization(false)
    {}

    // 也可以写成
    // const decltype(m_computation()) &operator() const
    operator const decltype(m_computation()) & () const
    {
        std::unique_lock<std::mutex> lock{m_cache_mutex};

        if (!m_cache_initialized) {
            m_cache = m_computation();
            m_cache_initialized = true;
        }

        return m_cache;
    }
};

template <typename F>
inline lazy_val<F> make_lazy_val(F &&computation)
{
    return lazy_val<F>(std::forward<F>(computation));
}
```

该程序在多线程上还是次优的，只有在第一次计算时才需要加锁。也可以使用`std::call_once`来解决

```c++
template<typename F>
class lazy_val
{
private:
    F m_computation;
    mutable decltype(m_computation()) m_cache;
    mutable std::once_flag m_value_flag;
public:
    ...
    operator const decltype(m_computation()) & () const
    {
        std::call_once(m_value_flag, [this] {
            m_cache = m_computation();
        });
        return m_cache;
    }
};
```

### 6.2 懒惰作为优化技巧

#### 6.2.1 懒惰的排序集合

假设你有一个存储在向量中的几百个雇员的集合。 你有一个窗口，可以一次显示10个雇员；用户可以选择根据各种标准对雇员进行排序，如姓名、年龄、为公司工作的年限等等。 当用户选择根据员工的年龄进行排序时，程序应该显示10名最年长的员工，并允许用户向下滚动以查看其余的排序列表。

你可以通过对整个集合进行排序并一次显示10名员工来轻松实现这一目标。尽管如果你希望用户对整个排序的列表感兴趣，这可能是一个很好的方法，但如果不是这样的话，这将是矫枉过正。用户可能只对前10个列表感兴趣，而每次排序的标准改变时，你都要对整个集合进行排序。

为了使其尽可能高效，你需要想出一种懒惰的方式来对集合进行排序。 你可以把这个算法建立在quicksort上，因为它是最常用的内存排序算法。

quicksort的基本变体所做的事情很简单：它从集合中抽取一个元素，将所有大于该元素的元素移到集合的开头，然后将其余的元素移到集合的结尾（你甚至可以使用`std::partition`来完成这个步骤）。 然后对两个新创建的分区重复这一步骤。

为了使算法变得懒惰--只对集合中需要显示给用户的部分进行排序，而对其余部分不进行排序，你应该怎么改？你不能避免做分区步骤，但你可以推迟对你不需要排序的部分集合的递归调用。

你只有在需要时才会对元素进行排序。 这样，你就避免了对数组中不需要排序的部分进行算法的递归调用。

**懒惰快速排序的复杂度**

因为C++标准规定了它所定义的所有算法的所需复杂度，有人可能对你刚才定义的懒惰quicksort算法的复杂度感兴趣。让集合的大小为n，并假设你想得到前k项。

对于第一个分区步骤，你需要O(n)个操作；对于第二个，O(n/2)；以此类推，直到达到你需要完全排序的分区大小。总的来说，对于分区，你在普通情况下有O(2n)，这与O(n)相同。

为了对大小为k的分区进行完全排序，你需要O(k log k)次操作，因为你需要执行常规的quicksort。因此，总的复杂度将是O(n + k log k)，这相当不错：这意味着如果你要搜索集合中的最大元素，你将和`std::max_element`算法处于同一水平。O(n)。而如果你要对整个集合进行排序，那将是O(n log n)，就像普通的quicksort算法。

#### 6.2.2 用户界面中的项目视图

虽然前面的例子的重点是展示如何修改一个特定的算法，使其更加懒惰，但你需要懒惰的原因是很常见的。每当你有大量的数据，但在有限的屏幕空间内向用户展示这些数据时，你就有机会通过偷懒来进行优化。一个常见的情况是，你的数据存储在某个数据库中，而你需要以这样或那样的方式向用户展示它。

为了重用上一个例子的想法，设想你有一个包含员工数据的数据库。当显示员工时，你想把他们的名字和他们的照片一起显示出来。在前面的例子中，你需要懒惰的原因是，当你需要一次只显示10个项目时，对整个集合进行排序是多余的。

现在你有一个数据库，它要为你做排序。这是否意味着你应该一次性加载所有的东西？当然不是。你没有做排序，但是数据库在做--就像懒惰排序一样，数据库倾向于在请求时对数据进行排序。如果你一次得到所有的员工，数据库将不得不对所有的员工进行排序。 但排序并不是唯一的问题。在你这边，你需要显示每个员工的图片--而加载图片需要时间和内存。如果你一次装入所有的东西，你会使你的程序慢下来，并使用太多的内存。

相反，常见的方法是懒散地加载数据--只有在需要向用户展示时才加载。这样一来，你就可以使用更少的处理器时间和更少的内存。唯一的问题是，你不可能完全偷懒。你将无法保存所有先前加载的员工照片，因为这样做又会需要太多的内存。你需要开始忘记以前加载的数据，并在再次需要它时重新加载它。

#### 6.2.3 通过缓存函数结果来修剪递归树

C++可以根据需要自定义懒惰程度


计算斐波那契数

```c++
unsigned int fib(unsigned int n)
{
    return n == 0 ? 0 :
           n == 1 ? 1 :
                    fib(n - 1) + fib(n - 2);
}
```

该实现方式效率很低，有两个循环调用，重复了计算

```c++
// 使用缓存方式

std::vector<unsigned int> cache{0, 1};

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
```

这种方法的好处就是不需要发明新的算法计算，因为`fib`函数是纯的，你甚至不需要知道其计算原理

计算斐波那契数列的高效缓存

```c++
// 如果不考虑多次计算，甚至只需要记录前两次的结果就可以实现了
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
```

#### 6.2.4 动态编程是一种懒惰的形式

动态编程（Dynamic programming）是一种通过将复杂的问题分割成许多小问题来解决的技术。当你解决这些较小的问题时，你存储它们的解决方案，这样你就可以在以后重新使用它们。这种技术被用于许多现实世界的算法中，包括寻找最短路径和计算字符串距离。

在某种意义上，你为计算第n个斐波那契数的函数所做的优化也是基于动态编程的。你有一个问题`fib(n)`，你把它分成两个小问题：`fib(n - 1)`和`fib(n - 2)`。(这对斐波那契数的定义很有帮助。)通过存储所有小问题的结果，你大大优化了初始算法。

虽然这是对`fib`函数的明显优化，但情况并非总是如此。让我们考虑计算两个字符串之间的相似性问题。其中一个可能的衡量标准是列文斯坦距离（Levenshtein）1（或编辑距离），它是将一个字符串转化为另一个字符串所需的最小的删除、插入和替换的数量。比如说：

- example和example——距离为0，字符串相同
- example和exam——距离为3，需要删除3个字符
- exam和eram——距离为1，需要把x换成r

递归的计算列文斯坦距离

```c++
unsigned int lev(unsigned int m, unsigned int n)
{
    return m == 0 ? n :
           n == 0 ? m :
           std::min({
                lev(m - 1, n) + 1,
                lev(m, n - 1) + 1,
                lev(m - 1, n - 1) + (a[m - 1] != b[n - 1])
           });
}
```

### 6.3 通用的记忆化

记忆化缓存

虽然通常为每个问题单独编写自定义缓存更好，这样你可以控制一个特定的值在缓存中停留多长时间（就像`fib(n)`的备忘录化版本的forgetful cache那样），并确定保持缓存的最佳结构（例如，对lev(m, n)使用矩阵），但有时能够将一个函数放入一个包装器并自动得到该函数的记忆化版本是很有用的。

```c++
template <typename Result, typename ... Args>
auto make_memoized(Result (*f) (Args ...))
{
    // 创建一个缓存，将参数通过map映射到计算结果上。
    // 如果你想在多线程环境中使用它，你需要用一个mutex来同步它的变化。
    std::map<std::tuple<Args...>, Result> cache;

    return [f, cache] (Args... args) mutable -> result
    {
        // 判断结果是否已经缓存
        const auto args_tuple = 
            std::make_tuple(args...);
        const auto cached = cache.find(args_tuple);
        if (cached == cache.end()) {
            // 没有缓存就计算结果缓存
            auto result = f(args...);
            cache[args_tuple] = result;
            return result;
        } else {
            // 有缓存直接返回缓存结果
            return cached->second;
        }
    }
}
```

简单来讲就是牺牲空间换取时间

记忆化后的`fib`函数版本使用

```c++
auto fibmemo = make_memoized(fib);

std::cout << "fib(15) = " << fibmemo(15)
          << std::endl;

std::cout << "fib(15) = " << fibmemo(15)
          << std::endl;
```

这个版本只会第二次执行得到优化，原因是`fib`函数里面没有调用缓存的版本

```c++
template <typename F>
unsigned int fib(F &&fibmemo, unsigned int n)
{
    return n == 0 ? 0
         : n == 1 ? 1
         : fibmemo(n - 1) + fibmemo(n - 2);
}
```

记忆化封装

```c++
class null_param {};

template <class Sig, class F>
class memoize_helper;

template <class Result, class... Args, class F>
class memoize_helper<Result (Args...), F>
{
private:
    using function_type = F;
    using args_tuple_type
        = std::tuple<std::decay_t<Args>...>;

    function_type f;
    mutable std::map<args_tuple_type, Result> m_cache;
    mutable std::recursive_mutex m_cache_mutex;

public:
    template <typename Function>
    memoize_helper(Function &&f, null_param) :
            f(f)
    {}

    memoize_helper(const memoize_helper &other) :
            f(other.f)
    {}

    template <class... InnerArgs>
    Result operator()(InnerArgs &&... args) const
    {
        std::unique_lock<std::recursive_mutex>
                lock{m_cache_mutex};
        
        const auto args_tuple = 
            std::make_tuple(args...);
        const auto cached = m_cache.find(args_tuple);
        if (cached != m_cache.end()) {
            return cached->second;
        } else {
            auto &&result = f(
                    *this,
                    std::forward<InnerArgs>(args)...);
            m_cache[args_tuple] = result;
            return result;
        }
    }
};

template <class Sig, class F>
memoize_helper<Sig, std::decay_t<F>>
make_memoized_r(F &&f)
{
    return {std::forward<F>(F), detail::null_param()};
}

// 创建
auto fibmemo = make_memoized_r<
            unsigned int(unsigned int)>(
                    [](auto &fib, unsigned int n) {
                        std::cout << "calculating " << n << "!\n";
                        return n == 0 ? 0
                             : n == 1 ? 1
                             : fib(n - 1) + fib(n - 2);
                    });
```

### 6.4 表达式模板和惰性字符串连接

一般字符串连接的实现方式

```c++
std::string fullname = title + " " + surname + ", " + name;
```

根据加法的左关联原则，可以看作

```c++
std::string fullname = (((title + " ") + surname) + ", ") + name;
```

该过程会创建销毁不需要的缓冲区域做临时计算

```c++
template <typename ...Strings>
class lazy_string_concat_helper;

template <typename LastString, typename ...Strings>
class lazy_string_concat_helper<LastString,
                                 Strings...>
{
private:
    // 存储原来字符串的拷贝
    LastString data;
    lazy_string_concat_helper<Strings...> tail;

public:
    lazy_string_concat_helper(
            LastString data,
            lazy_string_concat_helper<Strings...> tail) :
        data(data),
        tail(tail)
    {}

    int size() const
    {
        return data.size() + tail.size();
    }

    template <typename It>
    void save(It end) const
    {
        const auto begin = end - data.size();
        std::copy(data.cbegin(), data.cend(),
                  begin);
        tail.save(begin);
    }

    operator std::string() const
    {
        std::string result(size(), '\0');
        save(result.end());
        return result;
    }

    lazy_string_concat_helper<std::string,
                              LastString,
                              Strings...>
    operator+(const std::string &other) const
    {
        return lazy_string_concat_helper
            <std::string, LastString, Strings...>(
                other,
                *this
            );
    }
};

// 由于这个是递归的实现，为了避免无限循环迭代，还需要一个基础版本
tempalte <>
class lazy_string_concat_helper<>
{
public:
    lazy_string_concat_helper() {}

    int size() const
    {
        return 0;
    }

    template <typename It>
    void save(It) const {}

    lazy_string_concat_helper<std::string>
    operator+(const std::string &other) const
    {
        return lazy_string_concat_helper<std::string>(
                other,
                *this
            );
    }
};
```

使用

```c++
lazy_string_concat_helper<> lazy_concat;

int main(int argc, char *argv[])
{
    std::string name = "Jane";
    std::string surname = "Smith"

    const std::string fullname =
        lazy_concat + surname + ", " + name;

    std::cout << (std::string)fullname << std::endl;
}
```

#### 6.4.1 纯净和表达式模板

如果用引用存储，在真正获取到值之前对原来数据的修改都会生效

```c++
template <typename LastString, typename ...Strings>
class lazy_string_concat_helper<LastString,
                                 Strings...>
{
private:
    // 存储原来字符串的拷贝
    const LastString &data;
    lazy_string_concat_helper<Strings...> tail;

public:
    lazy_string_concat_helper(
            const LastString& data,
            lazy_string_concat_helper<Strings...> tail) :
        data(data),
        tail(tail)
    {}
    ...
};

// 未知原因段错误
int main()
{
    std::string name = "Jane";
    std::string surname = "Smith";
    const auto fullname =
            lazy_concat + surname + std::string(", ") + name;
    name = "John";
    std::cout << "begin print" << std::endl;
    std::cout << (std::string)fullname << std::endl;
}

```

## 7 范围

Ranges

根据人的归属划分组

```c++
template <typename Persons, typename F>
void group_by_team(Person &person,
                   F team_for_person,
                   const std::vector<std::string> &teams)
{
    auto begin = std::begin(persons);
    const auto end = std::end(persons);

    for (const auto &team : teams) {
        begin = std::partition(begin, end,
                [&] (const auto &person) {
                    return team == team_for_person(person);
                });
    }
}
```

主要的问题是，STL算法将一个集合的开头和结尾的迭代器作为单独的参数，而不是采取集合本身。这有一些影响：

- 这些算法不能作为结果返回一个集合。
- 即使你有一个返回集合的函数，你也不能直接把它传递给算法：你需要创建一个临时变量，这样你就可以对它调用开始和结束。
- 由于前面的原因，大多数算法都会对其参数进行变异，而不是让它们保持不可变，只是将修改后的集合作为结果返回。

这些因素使得在不具备至少是局部可改变变量的情况下，很难实现程序逻辑。

### 7.1 引入ranges

之前章节的一段代码

```c++
std::vector<std::string> names =
    transform(
        filter(people, is_female),
        name
    );
```

使用类似UNIX的管道语法规则，可以让代码更简洁

```c++
std::vector<std::string> names = people | filter(is_female)
                                        | transform(name);
```



### 7.2 创建只读的数据视图

过滤代理迭代器的增加操作符

```c++
auto &operator++()
{
    ++m_current_position;
    m_current_position =
        std::find_if(m_current_position,
                     m_end,
                     m_predicate);
    return *this;
}
```

#### 7.2.2 ranges的transform函数

转换代理迭代器的解引用运算操作

```c++
auto operator*() const
{
    return m_function(
            *m_current_position
        );
}
```

名字直接通过`m_function`解出来了

#### 7.2.4 range值的懒惰估计

evaluation，解算，估计

例子

```c++
std::vector<std::string> names = people | filter(is_female)
                                        | transform(name)
                                        | take(3);
```

分析：

- 当`people | filter(is_female)`估计，创建了一个新的view，没有从集合里面取得单一的`person`
- 把视图传给`| transform(name)`，创建了一个新的视图，仍然没有从里面取元素或者调用`name`函数
- 应用`| take(3)`，同样只是创建了一个新的视图
- 从`| take(3)`的结果构造字符串向量，进行计算

当构造向量时，进行了以下操作：

- 调用代理迭代器上面的解引用操作符，迭代器为`take`返回的范围
- `take`创建的代理迭代器将请求传递给`transform`创建的代理迭代器。该迭代器传递请求
- 试图解除对过滤器转换所定义的代理迭代器的引用。它穿过源集合，找到并返回第一个满足`is_female`谓词的人。这个是你第一次访问集合中的任何一个人，也是第一次调用`is_female`函数
- 通过解除引用过滤器代理迭代器检索到的人被传递`name`函数，结果被返回给`take`迭代器，后者将其名字传递到名字向量中插入

当插入一个元素后，下一个，然后再下一个，直到到达最后

<center>
    <p><img src="https://s1.ax1x.com/2022/04/21/LcFi5D.png"/></p>
</center>

### 7.3 ranges里面的可变值

前面的例子是不改变原集合，有些时候需要改变初始集合。我们把这些变换称为动作（actions），和视图（views）相对

动作转换的一个常见例子是排序。为了能够对一个集合进行排序，你需要访问它的所有元素并对它们重新排序。你需要改变原始集合，或者创建并保留整个集合的排序副本。当原始集合不是随机访问的（例如一个链表），并且不能有效地进行排序时，后者尤其重要；你需要把它的元素复制到一个可以随机访问的新集合中，并对它进行排序。

<<<<<<< HEAD
> 需要使用到ranges-v3库

统计所有出现过单词的集合
=======
需要用到`range-v3`库，在https://github.com/ericniebler/range-v3

给字符串排序去重复的例子
>>>>>>> f959a4b (add code)

```c++
std::vector<std::string> words =
        read_text() | action::sort
                    | action::unique;
```

视图和动作的这种组合使你有能力选择什么时候你想懒惰地完成某件事，什么时候你想急切地完成某件事。有这种选择的好处是，当你不期望源集合中的所有项目都需要处理时，以及当项目不需要被处理超过一次时，你可以偷懒；如果你知道结果集合中的所有元素会被经常访问，你可以急于计算它们。

### 7.4 使用划定的和无限的ranges

划定的(delimited)，无限的(infinite)

通常的测试是否到末尾的方法

```c++
auto i = std::begin(collection);
const auto end = std::end(collection);
for (; i != end; ++i) {
    // ...
}
```

并不需要迭代器，而是需要测试是否到末尾，这个特殊值叫做**哨兵**（sentinel)。可以更自由的测试是否到range的末尾

#### 7.4.1 使用划定的ranges优化处理输入ranges

一个划定的ranges是一个你事先不知道终点的范围，但你有一个谓词函数可以告诉你什么时候到达终点。比如字符串，根据`\0`来判断是否结束

还有比如用迭代器获取输入

```c++
std::accumulate(std::istream_iterator<double>(std::cin),
                    std::istream_iterator<double>(),
                    0);
```

> 该方法会一直接受`double`类型的输入，直到输入的类型不再为double

迭代器`std::istream_iterator`实现哨兵值的方式例如：

```c++
template<typename T>
bool operator==(const std::istream_iterator<T> &left,
                const std::istream_iterator<T> &right)
{
    if (left.is_sentinel() && right.is_sentinel()) { return true; }
    else if (left.is_sentinel()) {
        // 测试是否为哨兵谓词          
        // 对于右边的迭代器为真 
    } else if (right.is_sentinel()) {
        // 测试是否为哨兵谓词          
        // 对于左边的迭代器  
    } else {
        // 两个迭代器都为正常迭代器,          
        // 测试两个是否指向相同的位置
    }
}
```

#### 7.4.2 使用sentinels创建无限的ranges

sentinels方法为你提供了限定ranges的优化。但还有一点：你现在也能轻松地创建无限的ranges了。 无限ranges没有终点，就像所有正整数的范围。你有一个开始———数字0，但没有结束。

虽然你为什么需要无限的数据结构并不明显，但它们时常会派上用场。使用整数范围的一个最常见的例子是枚举另一个范围内的项目。 想象一下，你有一个按分数排序的电影范围，你想把前10部电影和它们的位置写到标准输出。

要做到这一点，你可以使用view::zip函数。它需要两个范围，并将这些范围中的项目配对。结果范围中的第一个元素将是一对项目：第一个范围的第一个项目和第二个范围的第一个项目。第二个元素将是一对，包含第一个范围的第二个项目和第二个范围的第二个项目，以此类推。一旦任何一个源范围结束，产生的范围就会结束。

```c++
template <typename Range>
void write_top_10(const Range &xs)
{
    auto items = 
            view::zip(xs, view::ints(1))
                | view::transform([] (const auto &pair) {
                      return std::to_string(pair.second) + " " + pair.first;
                  })
                | view::take(10);
    for (const auto &item : items) {
        std::cout << item << std::endl;
    }
}
```

当你在便利一个ranges但是只有在遍历的时候才知道有多长。你如果要根据长度遍历可能要遍历两次，第一次为了`view::zip`，第二次为了`view::transform`处理。

这样的另一个好处就是代码更加通用，算法支持在无限ranges情况下工作

## 7.5 使用ranges计算词频

`range-v3`库里面有`istream_range`可以把`std::cin`的输入转成ranges ~~，标准库里面有[`std::ranges::istream_view`](https://devdocs.io/cpp/ranges/basic_istream_view)~~

首先需要对输入字符串进行预处理，然后再进行筛选

```c++

std::string string_to_lower(const std::string &s)
{
return s | view::transform(tolower);
}

std::string string_only_alnum(const std::string &s)
{
return s | view::filter(isalnum);
}

std::vector<std::string> words =
        istream_range<std::string>(std::cin)
        | view::transform(string_to_lower)
        | view::transform(string_only_alnum)
        | view::remove_if(&std::string::empty);  // 类成员函数指针

const auto results =
        words | view::group_by(std::equal_to<>())
              | view::transform([] (cosnt auto &group) {
                    const auto begin = std::begin(group);
                    const auto end = std::end(group);
                    const auto count = std::distance(begin, end);
                    const auto word = *begin;
                    return std::make_pair(count, word);
                })
              | to_vector | action::sort;   //转成vector排序

```

> 这段代码怎么都编译不过！！

## 8 函数式数据结构

到目前为止，我主要谈的是更高层次的函数式编程概念，我们花了相当长的时间来研究没有可改变状态的编程的好处。问题是，程序往往有许多移动部件。在第五章讨论纯粹性的时候，我说过，其中一个选择是只让主要组件拥有可变的状态。所有其他组件都是纯粹的，并计算出一系列应该在主组件上执行的变化，但实际上并不改变任何东西。然后主组件可以对自己执行这些变化。

这种方法在程序的纯粹部分和处理可变状态的部分之间建立了明确的分离。问题是，设计这样的软件往往不容易，因为你需要注意功能数据结构计算出的状态变化的应用顺序。如果你没有正确地做到这一点，你可能会遇到类似于在并发环境中处理易变状态时的数据竞赛。

因此，有时有必要避免所有的可变--甚至没有中央可变的状态。如果你使用标准的数据结构，每当你需要一个新版本的数据时，你就必须复制这些数据。 每当你想改变一个集合中的一个元素时，你就需要创建一个新的集合，这个集合与旧的集合相同，但所需的元素已经改变。在使用标准库提供的数据结构时，这样做效率很低。在这一章中，我们将介绍那些可以有效复制的数据结构。创建一个数据结构的修改副本也将是一个高效的操作。

### 8.1 不可改变的链表

使用共享指针管理链表内部元素，然后链表的修改可以根据修改的位置减少总体的复制量

#### 8.2 不可改变的vector-like数据

(Copy-on-write), lazy copy

并不是每次都进行复制，而是对之前的数据进行切片记录到新的数据里面，直到最后需要进行转换写入才把真正的数据计算出来

## 参考

[^1]: [Partial Function Application in Haskell](https://blog.carbonfive.com/partial-function-application-in-haskell)

[^2]: [Lifting](https://wiki.haskell.org/Lifting)

[^3]: [What is referential transparency?](https://stackoverflow.com/questions/210835/what-is-referential-transparency)

[^4]: [阿姆达定律](https://zhuanlan.zhihu.com/p/48022905)
