
#pragma once

#include "position.hpp"
#include <set>
#include <map>

namespace bj {
    inline void print_game_of_life(const std::set<bj::Position> & position, int xMin, int xMax, int yMin, int yMax) {
        for (int y = yMin ; y <= yMax ; ++y) {
            for (int x = xMin ; x <= xMax ; ++x) {
                if (position.contains(bj::Position {x, y} )) {
                    std::cout << '#';
                } else {
                    std::cout << '.';
                }
            }
                std::cout << '\n';
        }
    }

    template <typename Rules>
    void game_of_life(std::set<typename Rules::Element> & positions, Rules rules) {
        using Element = Rules::Element;
        std::set<Element> old = std::move(positions);

        std::map<Element, size_t> counts;

        for (const auto & position : old) {
            counts[position];   // Ensure that currently living cells are in the map

            for (const auto & neighbour : rules.get_neighbours(position)) {
                ++counts[neighbour];
            }
        }

        for (const auto & [position, count] : counts) {
            if (!rules.is_valid(position)) continue;

            if (rules.is_on(position, old.contains(position), count)) {
                positions.insert(position);
            }
        }
    }

    template <typename Rules>
    [[nodiscard]] auto game_of_life(size_t loops, std::set<typename Rules::Element> positions, Rules rules) {
        for (size_t i = 0 ; i != loops; ++i) {
            game_of_life(positions, rules);
        }
        
        return positions;
    }
    
/*
    // Example of Rules implementation:

    struct Rules {
        using Element = bj::Position;
        int grid_size;

        [[nodiscard]] auto get_neighbours(const Element & element) const {
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


*/

}
