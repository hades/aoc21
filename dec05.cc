#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "absl/strings/numbers.h"
#include "boost/math/special_functions/sign.hpp"

using point_t = std::pair<int, int>;
using line_t = std::pair<point_t, point_t>;

int main() {
    std::ifstream input("c:/dev/aoc21/dec05.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    const std::basic_regex pattern(R"((\d+),(\d+) -> (\d+),(\d+))");
    std::map<point_t, int> counts_of_vents;
    std::map<point_t, int> counts_of_vents_with_diagonals;

    std::string line;
    while (std::getline(input, line)) {
        std::smatch match_results;
        if (!std::regex_match(line, match_results, pattern)) {
            std::cerr << "line does not match regex: " <<  line << std::endl;
            return 1;
        }

        line_t parsed_line;
        if (!!absl::SimpleAtoi(match_results[1].str(), &parsed_line.first.first) +
            !!absl::SimpleAtoi(match_results[2].str(), &parsed_line.first.second) +
            !!absl::SimpleAtoi(match_results[3].str(), &parsed_line.second.first) +
            !!absl::SimpleAtoi(match_results[4].str(), &parsed_line.second.second) != 4) {
            std::cerr << "failed to parse some of the numbers: " << line << std::endl;
        }

        if (parsed_line.first.first == parsed_line.second.first) {
            int delta = parsed_line.first.second < parsed_line.second.second ? 1 : -1;
            for (int i = parsed_line.first.second; i != parsed_line.second.second + delta; i += delta) {
                counts_of_vents[{parsed_line.first.first, i}]++;
                counts_of_vents_with_diagonals[{parsed_line.first.first, i}]++;
            }
        } else if (parsed_line.first.second == parsed_line.second.second) {
            int delta = parsed_line.first.first < parsed_line.second.first ? 1 : -1;
            for (int i = parsed_line.first.first; i != parsed_line.second.first + delta; i += delta) {
                counts_of_vents[{i, parsed_line.first.second}]++;
                counts_of_vents_with_diagonals[{i, parsed_line.first.second}]++;
            }
        } else {
            int dx = boost::math::sign(parsed_line.second.first - parsed_line.first.first);
            int dy = boost::math::sign(parsed_line.second.second - parsed_line.first.second);
            int count_x = std::abs(parsed_line.second.first - parsed_line.first.first);
            int count_y = std::abs(parsed_line.second.first - parsed_line.first.first);
            if (count_x != count_y) {
                std::cerr << "line is not diagonal: " << line << std::endl;
                return 1;
            }
            for (int i = 0; i <= count_x; ++i) {
                point_t point = parsed_line.first;
                point.first += dx*i;
                point.second += dy*i;
                counts_of_vents_with_diagonals[point]++;
            }
        }
    }
    std::cout << std::count_if(counts_of_vents.begin(), counts_of_vents.end(), [](auto& entry) { return entry.second > 1; }) << std::endl;
    std::cout << std::count_if(counts_of_vents_with_diagonals.begin(), counts_of_vents_with_diagonals.end(), [](auto& entry) { return entry.second > 1; }) << std::endl;
}
