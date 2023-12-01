#include "../../common/advent_of_code.hpp"

// https://adventofcode.com/2020/day/11


static unsigned int count_empty_seats(Board board) {
    unsigned int counter = 0;

    for (size_t x = 0 ; x != board.width() ; ++x) {
        for (size_t y = 0 ; y != board.height() ; ++y) {
            const auto c = board.get_at(x, y);
            if (*c == '#') {
                ++counter;
            }
        }
    }

    return counter;
}

static char around(char current_char, const Board & board, size_t x, size_t y) {
    // count occupied
    unsigned int occupied = 0;

    for (size_t xprime : { x - 1, x, x + 1 }) {
        for (size_t yprime : { y - 1, y, y + 1 }) {
            if (xprime == x && yprime == y) continue;

            if (const auto opt_char = board.get_at(xprime, yprime)) {
                // opt_char == std::nullopt if out of bound
                if (*opt_char == '#') {
                    ++occupied;
                }
            }
        }
    }

    // decision
    if (current_char == 'L') {
        return occupied == 0 ? '#' : 'L';
    } else {
        return occupied >= 4 ? 'L' : '#';
    }
}

static char line_vision(char current_char, const Board & board, const size_t x, const size_t y) {
    unsigned int occupied = 0;

    for (    int x_delta : { -1, 0, 1 }) {
        for (int y_delta : { -1, 0, 1 }) {
            if (x_delta == 0 && y_delta == 0) continue;

            size_t xp = x;
            size_t yp = y;

            while (true) {
                xp += x_delta;
                yp += y_delta;

                const auto opt_char = board.get_at(xp, yp);

                if (!opt_char) break;

                if (*opt_char != '.') {
                    if (*opt_char == '#') ++occupied;
                    break;
                }
            }
        }
    }

    if (current_char == 'L') {
        return occupied == 0 ? '#' : 'L';
    } else {
        return occupied >= 5 ? 'L' : '#';
    }
}

template <typename Function>
unsigned int figure_empty_seats(Board board, Function slot_changer) {
    while (true) {
        Board new_board = board;

        for (size_t x = 0 ; x != board.width() ; ++x) {
            for (size_t y = 0 ; y != board.height() ; ++y) {
                const char current_symbol = *board.get_at(x, y);
                if (current_symbol != '.') {
                    new_board.set_at(x, y, slot_changer(current_symbol, board, x, y));
                }
            }
        }

        if (new_board == board) {
            return count_empty_seats(board);
        }

        board = new_board;
    }
}

Output day_2020_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
    Board board = Board(lines);

    const unsigned int empty_seats_A = figure_empty_seats(board, around);
    const unsigned int empty_seats_B = figure_empty_seats(board, line_vision);

    return Output(empty_seats_A, empty_seats_B);
}
