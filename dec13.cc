#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"

using point_t = std::pair<int, int>;

int main() {
    std::ifstream input("c:/dev/aoc21/dec13.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    const std::basic_regex fold_expr(R"(fold along (.)=(\d+))");
    std::set<point_t> points;
    std::vector<std::pair<char, int>> fold_instructions;

    std::string line;
    while (std::getline(input, line)) {
        if (line.size() == 0) continue;

        std::smatch match_results;
        if (!std::regex_match(line, match_results, fold_expr)) {
            std::pair<absl::string_view, absl::string_view> pt = absl::StrSplit(line, ',');
            point_t point;
            if (!!absl::SimpleAtoi(pt.first, &point.first) +
                !!absl::SimpleAtoi(pt.second, &point.second) != 2) {
                std::cerr << "failed to parse some of the numbers: " << line << std::endl;
                return 2;
            }
            points.insert(point);
            continue;
        }

        fold_instructions.emplace_back();
        fold_instructions.back().first = match_results[1].str()[0];
        if (!absl::SimpleAtoi(match_results[2].str(), &fold_instructions.back().second)) {
            std::cerr << "failed to parse some of the numbers: " << line << std::endl;
            return 2;
        }
    }
    
    for (auto [axis, position]: fold_instructions) {
        std::set<point_t> new_points;
        for (auto [x, y]: points) {
            if (axis == 'x') {
                if (x < position) {
                    new_points.emplace(x, y);
                } else {
                    new_points.emplace(position + position - x, y);
                }
            } else {
                if (y < position) {
                    new_points.emplace(x, y);
                } else {
                    new_points.emplace(x, position + position - y);
                }
            }
        }
        std::cout << new_points.size() << std::endl;
        new_points.swap(points);
    }

    int max_x = 0, max_y = 0;
    for (auto [x, y]: points) {
        max_x = std::max(x, max_x);
        max_y = std::max(y, max_y);
    }

    for (int j = 0; j <= max_y; ++j) {
        for (int i = 0; i <= max_x; ++i) {
            std::cout << ((points.find({i, j}) != points.end())? "#": " ");
        }
        std::cout << std::endl;
    }
}