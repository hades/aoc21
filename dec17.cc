#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>

#include "absl/strings/numbers.h"

double solve_quadratic(double p, double q) {
    double p_half = p * 0.5;
    return -p_half + std::sqrt(p_half*p_half - q);
}

std::set<int> find_possible_xv0(int t, int x_min, int x_max) {
    std::set<int> result;
    // xv0 + (xv0 - 1) + ... + (xv0 - t + 1) = (xv0 + xv0 - t + 1) * t / 2
    // 2*xv0 - t + 1 = 2 * end_x / t
    // 2*xv0 = 2 * end_x / t + t - 1
    // xv0 = end_x / t + t/2 - 1/2
    int min_possible_xv0 = int(std::ceil(x_min / double(t) + t * 0.5 - 0.5));
    int max_possible_xv0 = int(std::floor(x_max / double(t) + t * 0.5 - 0.5));
    for (int xv0 = std::max(min_possible_xv0, t); xv0 <= max_possible_xv0; xv0++) {
        int end_x = (2*xv0 - t + 1) * t / 2;
        if (end_x >= x_min && end_x <= x_max)
            result.emplace(xv0);
    }
    int min_direct_xv0 = int(std::ceil(solve_quadratic(1, -2*x_min)));
    int max_direct_xv0 = int(std::floor(solve_quadratic(1, -2*x_max)));
    for (int xv0 = min_direct_xv0; xv0 <= std::min(t, max_direct_xv0); ++xv0) {
        int end_x = (xv0 + 1) * xv0 / 2;
        if (end_x >= x_min && end_x <= x_max)
            result.emplace(xv0);
    }
    return result;
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec17.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::string line;
    std::getline(input, line);

    std::smatch match_results;
    try {
        const std::basic_regex dest_area(R"(.*x=(-?\d+)[.][.](-?\d+), y=(-?\d+)[.][.](-?\d+))");
        if (!std::regex_match(line, match_results, dest_area)) {
            std::cerr << "does not match regex: " << line << std::endl;
            return 1;
        }
    } catch (std::regex_error e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    int x_min, x_max, y_min, y_max;
    if (!!absl::SimpleAtoi(match_results[1].str(), &x_min) +
        !!absl::SimpleAtoi(match_results[2].str(), &x_max) +
        !!absl::SimpleAtoi(match_results[3].str(), &y_min) +
        !!absl::SimpleAtoi(match_results[4].str(), &y_max) != 4) {
        std::cerr << "failed to parse numbers" << std::endl;
        return 1;
    }

    std::set<std::pair<int, int>> possible_starts;
    int max_height = 0;
    for (int possible_yv0 = -y_min; possible_yv0 >= 0; possible_yv0--) {
        // yv0 + (yv0 + 1) + ... + (yv0 + t - 1) = (yv0 + yv0 + t - 1) * t / 2
        // t^2 / 2 + (yv0 - 1/2) * t - y_end = 0
        // t^2 + (2*yv - 1) * t - 2*y_end = 0
        int min_possible_t = int(std::ceil(solve_quadratic(-1 + 2*possible_yv0, + 2*y_max)));
        int max_possible_t = int(std::floor(solve_quadratic(-1 + 2*possible_yv0, + 2*y_min)));
        int actual_yv0 = std::max(0, possible_yv0 - 1);
        // yv0 + (yv0 - 1) + ... + 1 = (1 + yv0) * yv0 / 2
        int height = (actual_yv0 * actual_yv0 + actual_yv0) / 2;
        max_height = std::max(max_height, height);

        for (int t = min_possible_t ; t <= max_possible_t; ++t) {
            for (int possible_xv0: find_possible_xv0(t, x_min, x_max)) {
                possible_starts.emplace(possible_xv0, -possible_yv0);
            }
            for (int possible_xv0: find_possible_xv0(t + 2*actual_yv0 + 1, x_min, x_max)) {
                possible_starts.emplace(possible_xv0, actual_yv0);
            }
        }
    }
    std::cout << max_height << std::endl;
    std::cout << possible_starts.size() << std::endl;
}