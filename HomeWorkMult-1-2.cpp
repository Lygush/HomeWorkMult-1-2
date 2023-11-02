#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
#include <execution>

void sum(std::vector<int> vector1, std::vector<int> vector2) {
    std::vector<int> sum_of_vec;
    for (int i{}; i < vector1.size(); ++i) {
        sum_of_vec.push_back(vector1.at(i) + vector2.at(i));
    }
}

int main() {
    std::cout << "Number of hardware cores: " << std::thread::hardware_concurrency() << "\n\n";
    std::vector<int> cores {1, 2, 4, 8, 16};
    std::vector<int> elements{1000, 10000, 100000, 1000000};
    for (const auto& i: elements) {
        std::cout << "\t" << i;
    }
    std::cout << "\n";

    for (const auto& c: cores) {
        std::cout << c << " cores "; 
        for (const auto& elem: elements) {
            std::vector<int> first_vec(elem);
            std::vector<int> second_vec(elem);
            std::mt19937 gen;
            std::uniform_int_distribution<int> dist(0,elem);
            std::generate(std::execution::par, first_vec.begin(), first_vec.end(), [&]() {
            return dist(gen);
            });
            std::generate(std::execution::par, second_vec.begin(), second_vec.end(), [&]() {
            return dist(gen);
            });
            
            int split{ elem / c };
            int begin{}, end{split};
            std::vector<std::vector<int>> vec1_pieces;
            std::vector<std::vector<int>> vec2_pieces;
            for (int i{}; i < c; ++i) {
                std::vector<int> temp1(first_vec.begin() + begin, first_vec.begin() + end);
                vec1_pieces.push_back(temp1);
                std::vector<int> temp2(second_vec.begin() + begin, second_vec.begin() + end);
                vec2_pieces.push_back(temp2);
                begin += split;
                end += split;
            }
            std::vector<std::thread> th_vec;
            auto start_t = std::chrono::steady_clock::now();
            for (int i{}; i < c; ++i) {
                th_vec.push_back(std::thread(sum, vec1_pieces.at(i), vec2_pieces.at(i)));
            }
            for (auto& j: th_vec) {
                j.join();
            }
            auto end_t = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end_t - start_t);
            std::cout << " " << diff.count() << "\t";
        }
        std::cout << "\n";
    }
}