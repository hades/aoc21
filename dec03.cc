#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::ifstream input("c:/dev/aoc21/dec03.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::vector<int> scores(12);
    std::vector<std::string> lines;
    for (std::string line; std::getline(input, line);) {
        for (int i = 0; i < 12; ++i) {
            if (line[i] == '1') scores[i]++;
            else scores[i]--;
        }
        lines.push_back(line);
    }
    int gamma = 0, epsilon = 0;
    for (int i = 0; i < 12; ++i) {
        if (scores[i] >= 0) {
            gamma = (gamma << 1) | 1;
            epsilon = epsilon << 1;
        } else {
            gamma = gamma << 1;
            epsilon = (epsilon << 1) | 1;
        }
    }
    std::vector<std::string> lines_for_ogr = lines;
    for (int pos = 0; lines_for_ogr.size() > 1 && pos < 12; pos++) {
        int score = 0;
        for (const auto& line: lines_for_ogr) {
            if (line[pos] == '1') score++;
            else score--;
        }
        char desired = score >= 0 ? '1' : '0';
        std::erase_if(lines_for_ogr, [desired, pos](const auto& line) { return line[pos] != desired; });
    }
    if (lines_for_ogr.size() != 1) {
        std::cerr << "couldn't filter the lines for OGR down to 1 line" << std::endl;
        return 1;
    }
    std::vector<std::string> lines_for_co = lines;
    for (int pos = 0; lines_for_co.size() > 1 && pos < 12; pos++) {
        int score = 0;
        for (const auto& line: lines_for_co) {
            if (line[pos] == '1') score++;
            else score--;
        }
        char desired = score >= 0 ? '0' : '1';
        std::erase_if(lines_for_co, [desired, pos](const auto& line) { return line[pos] != desired; });
    }
    if (lines_for_co.size() != 1) {
        std::cerr << "couldn't filter the lines for CO2 down to 1 line" << std::endl;
        return 1;
    }
    std::cout << gamma * epsilon << ' ' << std::stoi(lines_for_ogr[0], nullptr, 2) * std::stoi(lines_for_co[0], nullptr, 2) << std::endl;
}