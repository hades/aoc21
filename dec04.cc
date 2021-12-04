#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "absl/strings/str_split.h"
#include "absl/strings/numbers.h"
#include "absl/types/optional.h"

std::vector<int> read_numbers(std::ifstream& input) {
    std::string line;
    std::getline(input, line);
    std::vector<absl::string_view> vals = absl::StrSplit(line, ',');
    std::vector<int> result;
    std::transform(vals.begin(), vals.end(), std::back_inserter(result),
    [](auto val) {
        int result;
        absl::SimpleAtoi(val, &result);
        return result;
    });
    return result;
}

using cell_t = std::pair<int, bool>;
using board_t = std::vector<cell_t>;

absl::optional<board_t> read_board(std::ifstream& input) {
    board_t result;
    for (int i = 0; i < 25; ++i) {
        int val;
        if (input >> val) {
            result.push_back({val, false});
        } else {
            return absl::nullopt;
        }
    }
    return result;
}

void mark_number(board_t& board, int number) {
    for (auto& entry: board) {
        if (entry.first == number) entry.second = true;
    }
}

std::pair<bool, int> check_if_won(const board_t& board){
    bool won = false;
    for (int i = 0; i < 5; ++i) {
        bool horizontal_won = true;
        bool vertical_won = true;
        for (int j = 0; j < 5; ++j) {
            if (!board[i*5 + j].second) horizontal_won = false;
            if (!board[i + j*5].second) vertical_won = false;
        }
        won = won || horizontal_won || vertical_won;
        if (won) break;
    }
    if (!won) return {false, 0};
    int sum = 0;
    for (auto& cell: board) {
        if (!cell.second) sum += cell.first;
    }
    return {true, sum};
}

int main() {
    std::ifstream input("c:/dev/aoc21/dec04.in");
    if (!input) {
        std::cerr << "failed to open file: " <<  strerror(errno) << std::endl;
        return 1;
    }

    std::vector<int> numbers = read_numbers(input);
    std::vector<board_t> boards;
    while(input) {
        auto board = read_board(input);
        if (board) {
            boards.emplace_back(std::move(*board));
        }
    }

    bool first_won = false;
    std::set<board_t*> winner_boards;
    int last_winner_score;
    for (auto number : numbers) {
        for (auto& board : boards) {
            mark_number(board, number);
            auto [won, sum_unmarked] = check_if_won(board);
            if (won) {
                if (!first_won) {
                    std::cout << sum_unmarked * number << std::endl;
                }
                first_won = true;
                if (winner_boards.find(&board) == winner_boards.end()) {
                    last_winner_score = number * sum_unmarked;
                    winner_boards.insert(&board);
                }
            }
        }
    }
    std::cout << last_winner_score << std::endl;
}