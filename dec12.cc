#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "absl/strings/ascii.h"
#include "absl/strings/str_split.h"
#include "boost/range.hpp"

using point_t = std::pair<int, int>;
using line_t = std::pair<point_t, point_t>;

std::ostream& operator <<(std::ostream& out, const std::map<std::string, int>& map) {
    for (const auto& [edge, val]: map) {
        out << edge << "=" << val << ' ';
    }
    out << std::endl;
    return out;
}

int count_paths_recursively(std::string start_vertex,
                            const std::multimap<std::string, std::string>& edges,
                            std::set<std::string> visited_small) {
    if (start_vertex == "end") return 1;
    int result = 0;
    if (absl::AsciiStrToLower(start_vertex) == start_vertex) visited_small.insert(start_vertex);
    for (const auto& [_, to]: boost::make_iterator_range(edges.equal_range(start_vertex))) {
        if (visited_small.find(to) != visited_small.end()) continue;
        result += count_paths_recursively(to, edges, visited_small);
    }
    return result;
}

int count_paths_recursively_extra_visit(std::string start_vertex,
                            const std::multimap<std::string, std::string>& edges,
                            std::set<std::string> visited_small,
                            bool chosen_extra_visit) {
    if (start_vertex == "end") return 1;
    int result = 0;
    if (absl::AsciiStrToLower(start_vertex) == start_vertex) {
        visited_small.insert(start_vertex);
    }
    for (const auto& [_, to]: boost::make_iterator_range(edges.equal_range(start_vertex))) {
        if (visited_small.find(to) != visited_small.end()) {
            if (chosen_extra_visit) continue;
            if (to == "start") continue;
            result += count_paths_recursively_extra_visit(to, edges, visited_small, true);
            continue;
        }
        result += count_paths_recursively_extra_visit(to, edges, visited_small, chosen_extra_visit);
    }
    return result;
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec12.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::multimap<std::string, std::string> edges;

    std::string line;
    while (std::getline(input, line)) {
        std::pair<absl::string_view, absl::string_view> edge = absl::StrSplit(line, '-');
        edges.emplace(edge.second, edge.first);
        edges.insert(std::move(edge));
    }

    std::cout << count_paths_recursively("start", edges, {}) << std::endl;
    std::cout << count_paths_recursively_extra_visit("start", edges, {}, false) << std::endl;
}