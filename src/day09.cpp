#include "libs.hpp"

// https://adventofcode.com/2020/day/9


using UUINT = long long unsigned int;
using Values = std::vector<UUINT>;
using ValuesIter = Values::const_iterator;

static bool is_sum_of_two_in(ValuesIter begin, ValuesIter end, UUINT value) {
    for (auto it = begin; it != end; ++it) {
        for (auto it2 = begin; it2 != end; ++it2) {
            if (it == it2) continue;

            if (*it + *it2 == value) return true;
        }
    }

    return false;
}

static size_t find_invalid(const Values & values, const size_t preambule) {
    // Sliding window
    auto window_begin = values.begin();
    auto window_end = values.begin() + preambule;

    while (window_end != values.end()) {
        if (!is_sum_of_two_in(window_begin, window_end, *window_end)) {
            return window_end - values.begin();
        }

        ++window_begin;
        ++window_end;
    }

    return values.size();
}

static std::pair<ValuesIter, ValuesIter> get_contiguous_sum(const Values & values, const UUINT target) {
    ValuesIter it = values.begin();

    while (it != values.end()) {
        UUINT current_sum = 0;

        ValuesIter end = it;

        while (end != values.end() && current_sum < target) {
            current_sum += *end;
            ++end;
        }

        if (target == current_sum) {
            return std::pair(it, end);
        }

        ++it;
    }

    return std::pair(values.end(), values.end());
}

Output day09(const std::vector<std::string> & lines) {
    const std::vector<UUINT> values = lines_transform::map<UUINT>(lines,
        [](const std::string & s) { return std::stoull(s); }
    );
    const size_t preambule_size = values.size() == 20 ? 5 : 25;

    // Find number that is not the sum of the preambule_size previous elements
    const size_t position_of_invalid = find_invalid(values, preambule_size);

    // Find contiguous set of numbers which sum is the invalid number
    const auto [begin, end] = get_contiguous_sum(values, values[position_of_invalid]);
    const auto [min, max] = std::minmax_element(begin, end);
    
    return Output { values[position_of_invalid], *min + *max };
}
