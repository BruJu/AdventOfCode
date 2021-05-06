#include "../advent_of_code.hpp"
#include <algorithm>
#include <numeric>
#include <optional>
#include <cstring>
#include <array>
#include "../util/regex_helper.hpp"

// https://adventofcode.com/2015/day/24

// Input: some numbers
// We need to group these number in 3 different packs
// The sum of the numbers of each pack is the same
// Multiplying the numbers of a pack = Quantum Entanglement
// => Find the lowest possible Quantum Entanglement of the smallest possible
// pack

// Solution:
// N is the sum of the numbers
// We don't care about what actually are the two other packs, we just
// need to find the lowest product of numbers whose sum is N / 3, and with the
// less possible numbers
// 
// So that's just some graph exploration looking for the smallest nb_elements
// then smallest product with a target sum.

namespace {
    struct Solution {
        int64_t nb_elements = 0;
        int64_t product = 1;
        int64_t sum = 0;
    };

    using Iter = std::vector<int>::const_iterator;

    void find_solution(Solution & best, Solution current, Iter begin, Iter end) {
        // Prune
        if (best.nb_elements > 0) {
            if (best.nb_elements < current.nb_elements)
                return;

            if (best.nb_elements == current.nb_elements
                && best.product <= current.product)
                return;
        }

        // Is a candidate?
        if (current.sum == best.sum) {
            best.product = current.product;
            best.nb_elements = current.nb_elements;
            return;
        }

        // Need more values
        if (begin == end) return;

        find_solution(best, current, begin + 1, end);

        current.nb_elements += 1;
        current.sum     += *begin;
        current.product *= *begin;
        find_solution(best, current, begin + 1, end);
    }

    Solution find_solution(const std::vector<int> & values, int64_t target) {
        Solution best;
        best.sum = target;
        Solution solution;

        find_solution(best, solution, values.begin(), values.end());
        return best;
    }
}

Output day_2015_24(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<int> values = lines_transform::map<int>(lines, [](const std::string & s) { return std::stoi(s); });

    const auto sum = std::reduce(values.begin(), values.end());
    
    const auto solution_3 = find_solution(values, sum / 3); // 3 packs
    const auto solution_4 = find_solution(values, sum / 4); // 4 packs

    return Output(solution_3.product, solution_4.product);
}
