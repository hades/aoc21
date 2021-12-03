#include <fstream>
#include <iostream>
#include <string>

#include "absl/strings/str_split.h"
#include "absl/strings/numbers.h"

int main() {
    std::ifstream input("c:/dev/aoc21/dec02.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    int pos = 0, depth = 0;
    int a_depth = 0, a_aim = 0;
    for (std::string line; std::getline(input, line);) {
        std::vector<absl::string_view> vals = absl::StrSplit(line, absl::MaxSplits(' ', 1));
        int amount;
        if (!absl::SimpleAtoi(vals[1], &amount)) {
            std::cerr << "unexpected value in line: " << line;
            return 1;
        }
        if (vals[0] == "forward") {
            pos += amount;
            a_depth += (a_aim * amount);
        } else if (vals[0] == "down") {
            depth += amount;
            a_aim += amount;
        } else {
            depth -= amount;
            a_aim -= amount;
        }
    }
    std::cout << pos*depth << ' ' << pos*a_depth << std::endl;
}
