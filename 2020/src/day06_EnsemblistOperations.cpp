#include "../../common/advent_of_code.hpp"
#include "../../util/libs_ensemblist.hpp"

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

Output day_2020_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
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

    return Output(compute(set::union_<char>), compute(set::intersection<char>));
}
