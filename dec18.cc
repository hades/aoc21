#include <deque>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <string>

#include "absl/cleanup/cleanup.h"
#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include "absl/types/variant.h"

struct s_number_t;

using pair_t = std::pair<std::unique_ptr<s_number_t>, std::unique_ptr<s_number_t>>;

struct s_number_t {
    absl::variant<int, pair_t> data;

    s_number_t copy() {
        if (is_regular()) {
            return {absl::get<0>(data)};
        }

        auto& p = absl::get<1>(data);
        return {pair_t{std::make_unique<s_number_t>(p.first->copy()),
                       std::make_unique<s_number_t>(p.second->copy())}};
    }

    bool is_regular() const {
        return absl::holds_alternative<int>(data);
    }

    bool is_pair() const {
        return absl::holds_alternative<pair_t>(data);
    }
};

std::unique_ptr<s_number_t> make_regular(int value) {
    auto result = std::make_unique<s_number_t>();
    result->data = value;
    return result;
}

std::ostream& operator<<(std::ostream& out, const s_number_t& number) {
    if (number.is_regular()) {
        return out << absl::get<int>(number.data);
    }
    auto& data = absl::get<1>(number.data);
    return out << '[' << *data.first << ',' << *data.second << ']';
}

std::tuple<bool, int, int> explode_recursively(s_number_t& a, int depth) {
    if (a.is_regular()) return {false, 0, 0};
    auto& data = absl::get<1>(a.data);
    s_number_t* left = data.first.get();
    s_number_t* right = data.second.get();
    if (left->is_regular() && right->is_regular() && depth >= 4) {
        auto after_explode = absl::MakeCleanup([&a] {
            a.data = 0;
        });
        return {true, absl::get<0>(left->data), absl::get<0>(right->data)};
    }
    auto explosion_on_the_left = explode_recursively(*left, depth + 1);
    if (std::get<0>(explosion_on_the_left)) {
        s_number_t* d = right;
        while (d->is_pair()) {
            d = absl::get<1>(d->data).first.get();
        }
        absl::get<0>(d->data) += std::get<2>(explosion_on_the_left);
        return {true, std::get<1>(explosion_on_the_left), 0};
    }
    auto explosion_on_the_right = explode_recursively(*right, depth + 1);
    if (std::get<0>(explosion_on_the_right)) {
        s_number_t* d = left;
        while (d->is_pair()) {
            d = absl::get<1>(d->data).second.get();
        }
        absl::get<0>(d->data) += std::get<1>(explosion_on_the_right);
        return {true, 0, std::get<2>(explosion_on_the_right)};
    }
    return {false, 0, 0};
}

bool try_explode_first(s_number_t& a) {
    return std::get<0>(explode_recursively(a, 0));
}

bool try_split_first(s_number_t& a) {
    if (a.is_regular()) {
       int value = absl::get<0>(a.data);
       if (value >= 10) {
           int left = value / 2;
           a.data = pair_t{make_regular(left), make_regular(value - left)};
           return true;
       }
       return false;
    }
    auto& pair = absl::get<1>(a.data);
    return try_split_first(*pair.first) || try_split_first(*pair.second);
}

void reduce(s_number_t& a) {
    for (;;) {
        if (!try_explode_first(a) && !try_split_first(a)) return;
    }
}

s_number_t& operator+=(s_number_t& a, s_number_t&& b) {
    s_number_t result;
    result.data = pair_t{std::make_unique<s_number_t>(std::move(a)),
                         std::make_unique<s_number_t>(std::move(b))};
    a = std::move(result);
    reduce(a);
    return a;
}

absl::string_view parse_number(absl::string_view str, s_number_t& number) {
    if (str[0] >= '0' && str[0] <= '9') {
        number.data = str[0] - '0';
        str.remove_prefix(1);
        return str;
    }
    number.data = pair_t{std::make_unique<s_number_t>(), std::make_unique<s_number_t>()};
    str.remove_prefix(1);
    str = parse_number(str, *absl::get<1>(number.data).first);
    str.remove_prefix(1);
    str = parse_number(str, *absl::get<1>(number.data).second);
    str.remove_prefix(1);
    return str;
}

int64_t magnitude(const s_number_t& number) {
    if (number.is_regular()) {
        return absl::get<0>(number.data);
    }
    auto& data = absl::get<1>(number.data);
    return 3*magnitude(*data.first) + 2*magnitude(*data.second);
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec18.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::string line;
    std::vector<s_number_t> numbers;
    while (std::getline(input, line)) {
        numbers.emplace_back();
        parse_number(line, numbers.back());
    }
    s_number_t sum = numbers[0].copy();
    for (int i = 1; i < numbers.size(); ++i) {
        sum += numbers[i].copy();
    }
    std::cout << magnitude(sum) << std::endl;
    int64_t max_magnitude = 0;
    for (int i = 0; i < numbers.size(); ++i) {
        for (int j = 0; j < numbers.size(); ++j) {
            if (i == j) continue;
            s_number_t sum = numbers[i].copy();
            sum += numbers[j].copy();
            max_magnitude = std::max(max_magnitude, magnitude(sum));
        }
    }
    std::cout << max_magnitude << std::endl;
};