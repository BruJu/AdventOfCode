#include "../advent_of_code.hpp"

#include <numeric>

// https://adventofcode.com/2015/day/8

namespace {
    int count_escape_characters(const std::string & s) {
        const size_t size = s.size();

        size_t real = 0;
        bool next_is_escaped = false;

        for (size_t i = 1 ; i < s.size() - 1;) {
            if (!next_is_escaped && s[i] != '\\') {
                ++real;
                ++i;
            } else if (s[i] == '\\') {
                if (next_is_escaped) {
                    // \ symbol
                    ++real;
                    ++i;
                    next_is_escaped = false;
                } else {
                    next_is_escaped = true;
                    ++i;
                }
            } else if (s[i] == '"') {
                ++real;
                ++i;
                next_is_escaped = false;
            } else {
                // \x
                ++real;
                i += 3;
                next_is_escaped = false;
            }
        }

        return size - real;
    }

    int count_new_escape_characters(const std::string & s) {
        return 2 + std::count_if(s.begin(), s.end(), [](char c) { return c == '\\' || c == '"'; });
    }

    template <typename Mapper>
    size_t accumulate(const std::vector<std::string> & lines, Mapper mapper) {
        return std::accumulate(lines.begin(), lines.end(), 0, [&](int x, const std::string & rhs) { return x + mapper(rhs); });
    }
}

Output day_2015_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
    return Output(
        accumulate(lines, count_escape_characters), accumulate(lines, count_new_escape_characters)
    );
}
