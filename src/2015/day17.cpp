#include "../advent_of_code.hpp"
#include "../util/regex_helper.hpp"

#include <map>

// https://adventofcode.com/2015/day/17

namespace {
    size_t to(const std::vector<int> & containers, const std::vector<int> & leftovers, int target, size_t i = 0) {
        // Solution
        if (target == 0) return 1;

        // No more containers or available containers are not enough
        if (i == containers.size() || target > leftovers[i] || target < containers.back())
            return 0;

        // Exploration
        return to(containers, leftovers, target, i + 1) + to(containers, leftovers, target - containers[i], i + 1);
    }

    struct MinCont {
        size_t paths = 0;
        size_t number_of_containers = 0;

        friend MinCont operator+(const MinCont & lhs, const MinCont & rhs) {
            if (lhs.paths == 0) return rhs;
            if (rhs.paths == 0) return lhs;
            if (lhs.number_of_containers < rhs.number_of_containers) return lhs;
            if (lhs.number_of_containers > rhs.number_of_containers) return rhs;
            return MinCont { lhs.paths + rhs.paths, lhs.number_of_containers };
        }
    };

    MinCont to_minimize_containers(const std::vector<int> & containers, const std::vector<int> & leftovers, int target, size_t i = 0, size_t current_number_of_cont = 0) {
        // Solution
        if (target == 0) return MinCont { 1, current_number_of_cont };

        // No more containers or available containers are not enough
        if (i == containers.size() || target > leftovers[i] || target < containers.back())
            return MinCont { 0, current_number_of_cont };

        // Exploration
        const auto left = to_minimize_containers(containers, leftovers, target                , i + 1, current_number_of_cont);

        if (left.paths != 0 && left.number_of_containers == current_number_of_cont) { // Another branch cutting
            return left;
        }

        return left + to_minimize_containers(containers, leftovers, target - containers[i], i + 1, current_number_of_cont + 1);
    }
}

Output day_2015_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<int> containers = lines_transform::map<int>(lines, [](const std::string & s) { return std::stoi(s); });
    const int target = containers.back();
    containers.pop_back();
    std::sort(containers.rbegin(), containers.rend());

    std::vector<int> leftovers;
    for (auto container : containers) {
        for (auto & left : leftovers) {
            left += container;
        }
        leftovers.push_back(container);
    }

    const size_t qtt = to(containers, leftovers, target);
    const size_t qtt2 = to_minimize_containers(containers, leftovers, target).paths;

    return Output(qtt, qtt2);
}
