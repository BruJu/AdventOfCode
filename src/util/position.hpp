#pragma once

#include <optional>
#include <concepts>
#include <cmath>
#include <stdexcept>

namespace bj {
    enum class Direction { Left, Right, Top, Down };

    inline std::optional<Direction> to_direction_from_lrtd(const char symbol, const char * symbols) {
        if (symbol == symbols[0]) return Direction::Left;
        if (symbol == symbols[1]) return Direction::Right;
        if (symbol == symbols[2]) return Direction::Top;
        if (symbol == symbols[3]) return Direction::Down;
        return std::nullopt;
    }

    // Sortable position
    struct Position {
        int x = 0;
        int y = 0;

        [[nodiscard]] bool operator<(const Position & rhs) const {
            if (x < rhs.x) return true;
            if (x > rhs.x) return false;
            if (y < rhs.y) return true;
            if (y > rhs.y) return false;
            return false;
        }

        [[nodiscard]] bool operator==(const Position & rhs) const {
            return x == rhs.x && y == rhs.y;
        }
        
        [[nodiscard]] bool operator!=(const Position & rhs) const {
            return !(*this == rhs);
        }

        void move(Direction direction) {
            switch (direction) {
                case Direction::Left:   x -= 1; break;
                case Direction::Right:  x += 1; break;
                case Direction::Top:    y -= 1; break;
                case Direction::Down:   y += 1; break;
            }
        }

        template<typename Consumer>
        void for_each_neighbour(Consumer c) const {
            for (Direction d : { Direction::Left, Direction::Right, Direction::Down, Direction::Top }) {
                Position copy = *this;
                copy.move(d);
                c(copy);
            }
        }
        
        [[nodiscard]] std::vector<bj::Position> get_8_neighbours() const {
            std::vector<bj::Position> retval;

            for (int x_ = -1 ; x_ <= 1 ; ++x_) {
            for (int y_ = -1 ; y_ <= 1 ; ++y_) {
                if (y_ == 0 && x_ == 0) continue;
                retval.push_back(Position { x + x_, y + y_ });
            }}

            return retval;
        }

        template<std::invocable<bj::Position> Consumer>
        static void for_each_point_between(const bj::Position from, const bj::Position to, Consumer consumer) {
            if (from.x != to.x && from.y != to.y) {
                // Pure diagonal movements actually work
                if (std::abs(from.x - to.x) != std::abs(from.y - to.y)) {
                    throw std::runtime_error("bj::Position::for_each_point_between does not support oblique movements");
                }
            }

            const int delta_x = from.x < to.x ? 1 : from.x == to.x ? 0 : -1;
            const int delta_y = from.y < to.y ? 1 : from.y == to.y ? 0 : -1;

            bj::Position i = from;
            while (i != to) {
                consumer(i);
                i.x += delta_x;
                i.y += delta_y;
            }

            consumer(i);
        }
    };


    struct Rectangle {
        int left;
        int right;
        int top;
        int bottom;

        Rectangle(int left, int top, int right, int bottom)
        : left(left), right(right), top(top), bottom(bottom) {}

        template <typename Consumer>
        void for_each_position(Consumer consumer) const {
            for (int i = left ; i <= right ; ++i) {
                for (int j = top ; j <= bottom ; ++j) {
                    consumer(Position { i, j });
                }
            }
        }
    };

}