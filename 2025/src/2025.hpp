#include "../../common/advent_of_code.hpp"
#include <span>
#include <string>

template<typename FuncPartA, typename FuncPartB>
Output aoc_is_a_function(std::span<const std::string> lines, FuncPartA part_a, FuncPartB part_b) {
  std::int64_t sum_a = 0;
  std::int64_t sum_b = 0;

  for (const std::string & line : lines) {
    sum_a += part_a(line);
    sum_b += part_b(line);
  }

  return Output(sum_a, sum_b);
}
