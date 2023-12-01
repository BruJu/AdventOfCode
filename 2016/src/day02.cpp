#include "../../common/advent_of_code.hpp"
#include <array>

// https://adventofcode.com/2016/day/2

// Given a pad and directions, move in the pad and get the letter you are on

// One StructPad = one layout

namespace {
    struct SimplePad {
        int8_t x = 1;
        int8_t y = 1;

        void receive(char c) {
            if (c == 'U' && y > 0) --y;
            if (c == 'D' && y < 2) ++y;
            if (c == 'L' && x > 0) --x;
            if (c == 'R' && x < 2) ++x;
        }

        [[nodiscard]] char get() const {
            return '1' + x + y * 3;
        }
    };

    struct WeirdPad {
        static constexpr std::array<char, 5 * 5> layout {
             0 ,  0 , '1',  0 ,  0  ,
             0 , '2', '3', '4',  0  ,
            '5', '6', '7', '8', '9' ,
             0 , 'A', 'B', 'C',  0  ,
             0 ,  0,  'D',  0 ,  0  
        };

        int8_t x = 0;
        int8_t y = 2;

        void receive(char c) {
            // Backup
            auto old = *this;

            // Do the move
            if (c == 'U' && y > 0) --y;
            if (c == 'D' && y < 4) ++y;
            if (c == 'L' && x > 0) --x;
            if (c == 'R' && x < 4) ++x;

            // Revert if illegal
            if (get() == '\0') {
                *this = old;
            }
        }

        [[nodiscard]] char get() const {
            return layout[y * 5 + x];
        }
    };
}

Output day_2016_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::string first_result;
    std::string second_result;

    SimplePad pad;
    WeirdPad weird;
    for (const auto & line : lines) {
        for (char c : line) {
            pad.receive(c);
            weird.receive(c);
        }

        first_result += pad.get();
        second_result += weird.get();
    }

    return Output(
        first_result,
        second_result
    );
}
