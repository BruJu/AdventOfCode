#include <array>
#include "libs.hpp"

// https://adventofcode.com/2020/day/1

static std::optional<std::pair<int, int>> find_sum(
    const int target,
    std::vector<int>::iterator left,
    std::vector<int>::iterator right,
    const std::optional<std::vector<int>::iterator> forbidden = std::nullopt
) {
    --right;

    while (true) {
        if (left == right) {
            return std::nullopt;
        }

        if (forbidden) {
            if (*forbidden == left || *forbidden == right) {
                return std::nullopt;
            }
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
        auto f = find_sum(target - *middle, left, right, middle);

        if (f) {
            return std::array<int, 3>{f->first, f->second, *middle};
        }
    }

    return { -1, -1, -1 };
}

Output day01(const std::vector<std::string> & lines) {
    std::vector<int> numbers = lines_transform::to_ints(lines);
    std::sort(numbers.begin(), numbers.end());

    auto r1 = find_sum(2020, numbers.begin(), numbers.end());
    auto r2 = find_sum_three_values(2020, numbers.begin(), numbers.end());

    return Output(r1->first * r1->second, r2[0] * r2[1] * r2[2]);
}
