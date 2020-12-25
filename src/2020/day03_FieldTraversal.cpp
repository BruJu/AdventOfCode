#include "../advent_of_code.hpp"
#include <regex>

// https://adventofcode.com/2020/day/3

constexpr char TREE = '#';

static auto count_trees_on_slope(Board board, size_t x, size_t y, int dX, int dY) {
    long long int count = 0;

    BoardPosition pos = BoardPosition(board, x, y, true, false);

    while (auto something = *pos) {
        if (*something == TREE) {
            ++count;
        }

        pos.offset(dX, dY);
    }

    return count;
}

Output day_2020_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
    Board board = Board(lines);

    const auto slope_1_1 = count_trees_on_slope(board, 0, 0, 1, 1);
    const auto slope_3_1 = count_trees_on_slope(board, 0, 0, 3, 1);
    const auto slope_5_1 = count_trees_on_slope(board, 0, 0, 5, 1);
    const auto slope_7_1 = count_trees_on_slope(board, 0, 0, 7, 1);
    const auto slope_1_2 = count_trees_on_slope(board, 0, 0, 1, 2);

    const auto times = slope_1_1 * slope_3_1 * slope_5_1 * slope_7_1 * slope_1_2;

    return Output(slope_3_1 , times);
}
