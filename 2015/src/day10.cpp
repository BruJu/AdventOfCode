#include "../../common/advent_of_code.hpp"
#include <sstream>

// https://adventofcode.com/2015/day/10

namespace {
    struct Count {
        char c;
        int times;
    };

    std::string look_and_say(std::string line) {
        std::vector<Count> count;

        for (char c : line) {
            if (count.empty() || count.back().c != c) {
                count.push_back(Count { c, 1 });
            } else {
                ++count.back().times;
            }
        }

        std::ostringstream oss;

        for (const auto & [character, times] : count) {
            oss << times << character;
        }

        return oss.str();
    }
}

Output day_2015_10(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::string line = lines[0];

    for (size_t i = 0 ; i != 40 ; ++i) {
        line = look_and_say(line);
    }

    std::string line2 = line;
    for (size_t i = 0 ; i != 10 ; ++i) {
        line2 = look_and_say(line2);
    }

    return Output(line.size(), line2.size());
}
