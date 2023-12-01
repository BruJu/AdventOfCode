#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include "../../util/game_of_life.hpp"
#include <map>

// https://adventofcode.com/2015/day/18

namespace {
    struct Regular {
        using Element = bj::Position;
        int grid_size;

        [[nodiscard]] static auto get_neighbours(const Element & element) {
            return element.get_8_neighbours();
        }

        [[nodiscard]] bool is_valid(const Element & position) const noexcept {
            return position.x >= 0        && position.y >= 0
                && position.x < grid_size && position.y < grid_size;
        }

        [[nodiscard]] bool is_on(const Element &, const bool was_on, const size_t c) const noexcept {
            if (was_on) {
                return c == 2 || c == 3;
            } else {
                return c == 3;
            }
        }
    };

    struct BorderUp {
        using Element = bj::Position;
        int grid_size;

        [[nodiscard]] static auto get_neighbours(const Element & element) {
            return element.get_8_neighbours();
        }

        [[nodiscard]] bool is_valid(const bj::Position & position) const noexcept {
            return position.x >= 0        && position.y >= 0
                && position.x < grid_size && position.y < grid_size;
        }

        [[nodiscard]] bool is_on(const bj::Position & position, const bool was_on, const size_t c) const noexcept {
            if (is_coin(position)) return true;

            if (was_on) {
                return c == 2 || c == 3;
            } else {
                return c == 3;
            }
        }

        [[nodiscard]] bool is_coin(const bj::Position & position) const noexcept {
            const bool coin_x = position.x == 0 || position.x == grid_size - 1;
            const bool coin_y = position.y == 0 || position.y == grid_size - 1;
            return coin_x && coin_y;
        }
    };
}

Output day_2015_18(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
    std::set<bj::Position> positions;
    for (int y = 0 ; y != static_cast<int>(lines.size()) ; ++y) {
        for (int x = 0 ; x != static_cast<int>(lines[0].size()) ; ++x) {
            if (lines[y][x] == '#') {
                positions.insert(bj::Position { x, y });
            }
        }
    }

    const size_t part_a = game_of_life(dei.part_a_extra_param,
        positions, Regular  { static_cast<int>(lines.size()) }
    ).size();

    const int bound = lines.size() - 1;
    positions.insert(bj::Position { 0    , 0     });
    positions.insert(bj::Position { bound, 0     });
    positions.insert(bj::Position { bound, bound });
    positions.insert(bj::Position { 0    , bound });
    const size_t part_b = game_of_life(dei.part_b_extra_param,
        positions, BorderUp { static_cast<int>(lines.size()) }
    ).size();

    return Output(part_a, part_b);
}
