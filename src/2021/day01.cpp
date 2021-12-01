#include "../advent_of_code.hpp"
#include <numeric>

Output day_2021_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<int> numbers = lines_transform::to_ints(lines);
  
  // Part A
  size_t part_a = 0;
  for (auto it = numbers.begin() + 1; it != numbers.end(); ++it) {
    if (*it > *(it - 1)) {
      ++part_a;
    }
  }

  // Part B
  static constexpr size_t sliding_window = 3;
  size_t part_b = 0;
  int current_sum = std::reduce(numbers.begin(), numbers.begin() + sliding_window);

  for (auto it = numbers.begin() + sliding_window; it != numbers.end(); ++it) {
    int new_sum = current_sum - *(it - sliding_window) + *it;
    if (new_sum > current_sum) {
      ++part_b;
    }
    current_sum = new_sum;
  }

  return Output(part_a, part_b);
}
