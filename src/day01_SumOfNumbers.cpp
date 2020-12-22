#include "libs.hpp"

#include <array>
#include <unordered_set>

// https://adventofcode.com/2020/day/1

static std::array<int, 2> find_sum_two(int target, const std::vector<int> & values) {
    std::unordered_set<int> expected_values;

    for (int value : values) {
        if (expected_values.contains(target - value)) {
            return { value , target - value };
        }

        expected_values.insert(value);
    }

    return { -1, -1 };
}

static std::optional<std::pair<int, int>> find_sum_three_values_(
    const int target,
    std::vector<int>::iterator left,
    std::vector<int>::iterator right,
    std::vector<int>::iterator forbidden
) {
    --right;

    while (true) {
        if (forbidden == left || forbidden == right) {
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
    for (auto middle = left ; middle != right ; ++middle) {
        auto f = find_sum_three_values_(target - *middle, left, right, middle);

        if (f) {
            return std::array<int, 3>{f->first, f->second, *middle};
        }
    }

    return { -1, -1, -1 };
}

Output day01(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<int> numbers = lines_transform::to_ints(lines);
    
    const auto r1 = find_sum_two(2020, numbers);

    std::sort(numbers.begin(), numbers.end());
    const auto r2 = find_sum_three_values(2020, numbers.begin(), numbers.end());

    return Output(r1[0] * r1[1], r2[0] * r2[1] * r2[2]);
}
