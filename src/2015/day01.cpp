#include "../advent_of_code.hpp"

// https://adventofcode.com/2015/day/1

static std::pair<int, size_t> count(std::string_view line) {
    int floor = 0;
    size_t basement = 0;
    for (size_t i = 0 ; i != line.size() ; ++i) {
        char c = line[i];

        if (c == '(') {
            ++floor;
        } else if (c == ')') {
            --floor;
        }

        if (floor == -1 && !basement) {
            basement = i + 1;
        }
    }
    return { floor, basement };
}

Output day_2015_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::string & line = lines[0];
    
    const auto [parenthesis, basement] = count(line);

    return Output(parenthesis, basement);
}
