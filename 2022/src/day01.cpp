#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <numeric>

// Given a list of list of numbers:
// - (A) Find the max
// - (B) Sum the 3 biggest values

// Example of input:
// 1
// 2
//
// 5
//
// 6
// 7
// 2

Output day_2022_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Process the input
  std::vector<int> elves = lines_transform::group<int, int>(
    /* input */           lines,
    /* line convertion */ [](const std::string & s) { return std::stoi(s); },
    /* reducer */         [](const int lhs, const int rhs) { return lhs + rhs; },
    /* finalizer */       lines_transform::map_identity<int>
  );

  // Find the 3 maxes
  const auto last_interesting_elve = elves.begin() + 3;
  std::partial_sort(
    elves.begin(), last_interesting_elve, elves.end(),
    std::greater<int>{}
  );
  
  // Compute output
  const size_t part_a = elves[0];
  const size_t part_b = std::reduce(elves.begin(), last_interesting_elve);

  // Done gg
  return Output(part_a, part_b);
}
