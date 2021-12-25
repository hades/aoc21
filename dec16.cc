#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>

#include "absl/strings/numbers.h"
#include "absl/types/variant.h"

uint64_t pop_bits(std::deque<char>& q, size_t count) {
    uint64_t result = 0;
    for (size_t i = 0; i < count; ++i) {
        result = (result << 1) | q.front();
        q.pop_front();
    }
    return result;
}

constexpr uint64_t type_literal = 4;

struct packet {
    uint64_t version;
    uint64_t type;
    absl::variant<uint64_t, std::vector<std::unique_ptr<packet>>> data;
};

std::ostream& operator<<(std::ostream& out, const packet& p) {
    out << p.version << '/' << p.type << '[';
    if (absl::holds_alternative<uint64_t>(p.data)) {
        out << absl::get<uint64_t>(p.data);
    } else {
        for (const auto& sp: absl::get<std::vector<std::unique_ptr<packet>>>(p.data)) {
            out << *sp << ' ';
        }
    }
    out << ']';
}

packet parse_packet(std::deque<char>& q) {
    packet result;
    result.version = pop_bits(q, 3);
    result.type = pop_bits(q, 3);
    if (type_literal == result.type) {
        uint64_t& data = absl::get<uint64_t>(result.data);
        for (;;) {
            uint64_t chunk = pop_bits(q, 5);
            data = (data << 4) | (chunk & 0xf);
            if (!(chunk & 0x10)) break;
        }
        return result;
    }
    std::vector<std::unique_ptr<packet>> data;
    bool is_packet_count = pop_bits(q, 1);
    if (is_packet_count) {
        uint64_t packet_count = pop_bits(q, 11);
        for (size_t i = 0; i < packet_count; ++i) {
            data.push_back(std::make_unique<packet>(parse_packet(q)));
        }
    } else {
        uint64_t bit_count = pop_bits(q, 15);
        std::deque<char> nq;
        for (size_t i = 0; i < bit_count; ++i) {
            nq.push_back(q.front());
            q.pop_front();
        }
        while (!nq.empty()) {
            data.push_back(std::make_unique<packet>(parse_packet(nq)));
        }
    }
    result.data = std::move(data);
    return result;
}

uint64_t sum_versions(const packet& p) {
    uint64_t result = p.version;
    if (absl::holds_alternative<std::vector<std::unique_ptr<packet>>>(p.data)) {
        for (const auto& sp: absl::get<1>(p.data)) {
            result += sum_versions(*sp);
        }
    }
    return result;
}

uint64_t execute(const packet& p) {
    switch (p.type) {
    case 0: {
        uint64_t sum = 0;
        for (const auto& sp: absl::get<1>(p.data)) {
            sum += execute(*sp);
        }
        return sum;
    }
    case 1: {
        uint64_t product = 1;
        for (const auto& sp: absl::get<1>(p.data)) {
            product *= execute(*sp);
        }
        return product;
    }
    case 2: {
        std::vector<uint64_t> datums;
        for (const auto& sp: absl::get<1>(p.data)) {
            datums.push_back(execute(*sp));
        }
        return *std::min_element(datums.begin(), datums.end());
    }
    case 3: {
        std::vector<uint64_t> datums;
        for (const auto& sp: absl::get<1>(p.data)) {
            datums.push_back(execute(*sp));
        }
        return *std::max_element(datums.begin(), datums.end());
    }
    case 4:
        return absl::get<0>(p.data);
    case 5: {
        std::vector<uint64_t> datums;
        for (const auto& sp: absl::get<1>(p.data)) {
            datums.push_back(execute(*sp));
        }
        return datums[0] > datums[1] ? 1 : 0;
    }
    case 6: {
        std::vector<uint64_t> datums;
        for (const auto& sp: absl::get<1>(p.data)) {
            datums.push_back(execute(*sp));
        }
        return datums[0] < datums[1] ? 1 : 0;
    }
    case 7: {
        std::vector<uint64_t> datums;
        for (const auto& sp: absl::get<1>(p.data)) {
            datums.push_back(execute(*sp));
        }
        return datums[0] == datums[1] ? 1 : 0;
    }
    }
    std::cerr << "unknown operator " << p.type << std::endl;
    return 0;
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec16.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::string line;
    std::getline(input, line);
    
    std::deque<char> bits;
    for (char ch: line) {
        absl::int128 number;
        if (!absl::SimpleHexAtoi(absl::string_view(&ch, 1), &number)) {
            std::cerr << "NaN: " << ch << std::endl;
            return 1;
        }
        for (int shift = 3; shift >= 0; --shift) {
            bits.push_back(char{(number >> shift) & 1});
        }
    }

    packet p = parse_packet(bits);
    std::cout << sum_versions(p) << std::endl;
    std::cout << execute(p) << std::endl;
}