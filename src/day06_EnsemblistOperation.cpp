#include "libs.hpp"

#include <numeric>
#include <set>

// https://adventofcode.com/2020/day/6


static std::set<char> to_set(const std::string & x) {
    std::set<char> retval;
    for (char xx : x) {
        retval.insert(xx);
    }
    return retval;
}

static std::set<char> union_op(const std::set<char> & lhs, const std::set<char> & rhs) {
    std::set<char> retval = lhs;
    retval.insert(rhs.begin(), rhs.end());
    return retval;
}

static std::set<char> intersection_op(const std::set<char> & lhs, const std::set<char> & rhs) {
    std::set<char> retval;

    for (char x : lhs) {
        if (rhs.contains(x)) {
            retval.insert(x);
        }
    }

    return retval;
}

Output day06(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const auto compute = [&](auto reducer) {
        const std::vector<size_t> per_group =
            lines_transform::group<size_t, std::set<char>>(
                lines,
                to_set,     // string -> std::set<char>
                reducer,    // 2 std::set<char> -> 1 std::set<char>
                [](const std::set<char> & m) { return m.size(); } // std::set::size
            );

        return std::accumulate(per_group.begin(), per_group.end(), 0);
    };

    return Output(compute(union_op), compute(intersection_op));
}
