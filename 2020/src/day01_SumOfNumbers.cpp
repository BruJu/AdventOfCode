#include "../../common/advent_of_code.hpp"

#include <array>
#include <unordered_set>

// https://adventofcode.com/2020/day/1

// find_sum_two_set and find_sum_two_sorted have the same behavior

static std::array<int, 2> find_sum_two_set(int target, const std::vector<int> & values) {
    std::unordered_set<int> expected_values;

    for (int value : values) {
        if (expected_values.contains(target - value)) {
            return { value , target - value };
        }

        expected_values.insert(value);
    }

    return { -1, -1 };
}

static std::optional<std::pair<int, int>> find_sum_two_sorted(
    const int target,
    std::vector<int>::iterator left,
    std::vector<int>::iterator right
) {
    --right;

    while (true) {
        if (left == right) {
            return std::nullopt;
        }

        const int sum = *left + *right;

        if (sum < target) {
            ++left;
        } else if (sum > target) {
            --right;
        } else {
            return std::pair<int, int>(*left, *right);
        }
    }
}

static std::array<int, 3> find_sum_three_values(
    const int target,
    std::vector<int>::iterator left,
    std::vector<int>::iterator right
) {
    while (left != right) {
        if (const auto f = find_sum_two_sorted(target - *left, left + 1, right)) {
            return std::array<int, 3>{f->first, f->second, *left};
        }

        ++left;
    }

    return { -1, -1, -1 };
}

Output day_2020_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<int> numbers = lines_transform::to_ints(lines);
    
    const auto r1 = find_sum_two_set(2020, numbers);

    std::sort(numbers.begin(), numbers.end());
    const auto r2 = find_sum_three_values(2020, numbers.begin(), numbers.end());

    return Output(r1[0] * r1[1], r2[0] * r2[1] * r2[2]);
}
