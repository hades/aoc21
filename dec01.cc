#include <deque>
#include <fstream>
#include <iostream>
#include <numeric>

#include "absl/strings/str_join.h"
#include "absl/types/optional.h"

template <typename i>
i sum(const std::deque<i>& c) {
    return std::accumulate(c.begin(), c.end(), 0);
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec01.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::deque<int> buf;
    int increases_1 = 0;
    int increases_3 = 0;
    for (int next_value; input >> next_value;) {
        if (buf.size() > 0 && buf.front() < next_value) increases_1++;
        buf.push_front(next_value);
        if (buf.size() < 4) continue;
        int prev_sum = sum(buf) - next_value;
        int next_sum = prev_sum - buf.back() + next_value;
        if (next_sum > prev_sum) increases_3++;
        buf.pop_back();
    }
    std::cout << increases_1 << ' ' << increases_3 << std::endl;
}