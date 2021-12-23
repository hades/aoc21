#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "absl/strings/str_split.h"

int64_t frequencies_amplitude(const std::map<std::string, int64_t>& cur_string, char end) {
    std::map<char, int64_t> frequencies;
    frequencies[end] = 1;
    for (const auto& [pair, count]: cur_string) {
        frequencies[pair[0]] += count;
    }
    auto comparator = [](const auto& a, const auto& b) {
        return a.second < b.second;
    };
    int64_t least_common = std::min_element(frequencies.begin(), frequencies.end(), comparator)->second;
    int64_t most_common = std::max_element(frequencies.begin(), frequencies.end(), comparator)->second;
    return most_common - least_common;
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec14.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::string start;
    std::getline(input, start);

    std::map<std::string, char> rules;

    std::string line;
    std::getline(input, line);
    while (std::getline(input, line)) {
        std::pair<std::string, std::string> rule = absl::StrSplit(line, " -> ");
        rules[rule.first] = rule.second[0];
    }

    std::map<std::string, int64_t> cur_string;
    for (int j = 0; j < start.size() - 1; ++j) {
        std::string pair{start[j], start[j+1]};
        cur_string[pair]++;
    }

    for (int i = 0; i < 10; ++i) {
        std::map<std::string, int64_t> next_string;
        for (const auto& [pair, count]: cur_string) {
            std::string pair1{pair[0], rules[pair]};
            std::string pair2{rules[pair], pair[1]};
            next_string[pair1] += count;
            next_string[pair2] += count;
        }
        cur_string.swap(next_string);
    }
    char end = start[start.size() - 1];
    std::cout << frequencies_amplitude(cur_string, end) << std::endl;
    for (int i = 0; i < 30; ++i) {
        std::map<std::string, int64_t> next_string;
        for (const auto& [pair, count]: cur_string) {
            std::string pair1{pair[0], rules[pair]};
            std::string pair2{rules[pair], pair[1]};
            next_string[pair1] += count;
            next_string[pair2] += count;
        }
        cur_string.swap(next_string);
    }
    std::cout << frequencies_amplitude(cur_string, end) << std::endl;
}