#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <string>

using point_t = std::pair<int, int>;

int dijkstra(const std::map<point_t, int>& risk_levels, const point_t destination) {
    std::priority_queue<std::pair<int, point_t>> q;
    std::set<point_t> unvisited;
    std::transform(risk_levels.begin(), risk_levels.end(), std::inserter(unvisited, unvisited.end()), [](auto entry) {
        return entry.first;
    });
    std::map<point_t, int> distances{{{0, 0}, 0}};
    q.emplace(0, point_t{0, 0});
    while (!unvisited.empty()) {
        point_t point = q.top().second;
        q.pop();
        if (unvisited.find(point) == unvisited.end()) continue;
        unvisited.erase(point);
        if (point == destination) break;

        for (auto [dx, dy]: std::initializer_list<std::pair<int, int>>{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
            point_t next = {point.first + dx, point.second + dy};
            if (risk_levels.find(next) == risk_levels.end()) continue;
            int new_distance = distances[point] + risk_levels.at(next);
            auto old_distance = distances.find(next);
            if (old_distance == distances.end() || old_distance->second > new_distance) {
                distances[next] = new_distance;
                q.emplace(-new_distance, next);
            }
        }
    }
    return distances[destination];
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec15.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::map<point_t, int> risk_levels;
    int max_x = 0, max_y = 0;
    int i = 0;
    std::string line;
    while (std::getline(input, line)) {
        int j = 0;
        for (char ch: line) {
            risk_levels[{i, j}] = ch - '0';
            max_x = std::max(max_x, j);
            j++;
        }
        max_y = std::max(max_y, i);
        i++;
    }

    std::cout << dijkstra(risk_levels, point_t{max_x, max_y}) << std::endl;
    std::map<point_t, int> big_risk_levels;
    for (auto [point, level]: risk_levels) {
        for (int dx = 0; dx < 5; ++dx) {
            for (int dy = 0; dy < 5; ++dy) {
                point_t new_point = {point.first + (max_x+1) * dx, point.second + (max_y+1) * dy};
                big_risk_levels[new_point] = (level - 1 + dx + dy) % 9 + 1;
            }
        }
    }
    std::cout << dijkstra(big_risk_levels, point_t{(max_x+1)*5 - 1, (max_y+1)*5 - 1}) << std::endl;
}