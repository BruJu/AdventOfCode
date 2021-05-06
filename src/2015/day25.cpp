#include "../advent_of_code.hpp"
#include <algorithm>
#include <numeric>
#include <optional>
#include <cstring>
#include <array>
#include "../util/regex_helper.hpp"

// https://adventofcode.com/2015/day/25

namespace {
    struct Position {
        int x = 1;
        int y = 1;

        Position & operator++() {
            if (y == 1) {
                y += x;
                x = 1;
            } else {
                x += 1;
                y -= 1;
            }
            return *this;
        }

        [[nodiscard]] bool operator!=(const Position & other) const noexcept {
            return x != other.x || y != other.y;
        }
    };
}

Output day_2015_25(const std::vector<std::string> &, const DayExtraInfo &) {
    int64_t value = 20151125;
    Position p;

    Position goal { 3075, 2981 };

    while (p != goal) {
        value *= int64_t(252533);
        value %= int64_t(33554393);
        ++p;
    }

    return Output(value, 0);
}
