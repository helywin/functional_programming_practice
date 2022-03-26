/*********************************************************************************
 * FileName: calculate_average.cpp
 * Author: helywin(jiang770882022@hotmail.com)
 * Version: 0.1
 * Date: 2022/3/26
 * Description: 
 * Others:
*********************************************************************************/

#include <vector>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <execution>

double average_score(const std::vector<int> &scores)
{
    int sum = 0;
    for (int score: scores) {
        sum += score;
    }
    return sum / (double) scores.size();
}

double average_score1(const std::vector<int> &scores)
{
    return std::accumulate(scores.cbegin(), scores.cend(), 0) /
           (double) scores.size();
}

double average_score2(const std::vector<int> &scores)
{
    return std::reduce(
            std::execution::par,
            scores.cbegin(), scores.cend(),
            0
    ) / (double) scores.size();
}

double scores_product(const std::vector<int> &scores)
{
    return std::accumulate(
            scores.cbegin(), scores.cend(),
            1,
            std::multiplies()
    );
}

int main()
{
    std::vector<int> vals{1, 3, 2, 7, 5, 6, 7,};
    std::cout << "average: " << average_score(vals) << std::endl;
    std::cout << "average: " << average_score1(vals) << std::endl;
    std::cout << "average: " << average_score2(vals) << std::endl;
    std::cout << "products: " << scores_product(vals) << std::endl;
}