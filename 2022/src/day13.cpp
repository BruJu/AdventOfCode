#include "../../common/advent_of_code.hpp"
#include "../../util/nested_ints.hpp"
#include <compare>
#include <exception>

// https://adventofcode.com/2022/day/13
// JS : https://gist.github.com/BruJu/99d606be171c79c9c9ef255dfb2d4ea0

using NestedInts = bj::nested_ints::NestedInts;

static std::partial_ordering compare_packet(
  const NestedInts & lhs,
  const NestedInts & rhs
) {
  if (lhs.is_integer && rhs.is_integer) {
    if (lhs.direct_int < rhs.direct_int) return std::partial_ordering::less;
    if (lhs.direct_int > rhs.direct_int) return std::partial_ordering::greater;
    return std::partial_ordering::equivalent;
  } else if (lhs.is_integer) {
    return compare_packet(NestedInts(std::vector<NestedInts>{ NestedInts(lhs.direct_int) }), rhs);
  } else if (rhs.is_integer) {
    return compare_packet(lhs, NestedInts(std::vector<NestedInts>{ NestedInts(rhs.direct_int) }));
  } else {
    for (size_t i = 0; i != lhs.nested.size(); ++i) {
      if (i >= rhs.nested.size()) return std::partial_ordering::greater;

      const auto res_on_i = compare_packet(lhs.nested[i], rhs.nested[i]);
      if (res_on_i != std::partial_ordering::equivalent) return res_on_i;
    }

    if (lhs.nested.size() < rhs.nested.size()) return std::partial_ordering::less;
    return std::partial_ordering::equivalent;
  }
}

static bool is_less(const NestedInts & lhs, const NestedInts & rhs) {
  return compare_packet(lhs, rhs) == std::partial_ordering::less;
}

Output day_2022_13(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<NestedInts> values;

  for (const std::string & line : lines) {
    if (line == "") continue;
    values.emplace_back(bj::nested_ints::read(line));
  }

  size_t part_a = 0;
  for (size_t i = 0; i < values.size(); i += 2) {
    if (is_less(values[i], values[i + 1])) {
      const size_t pair_id = (i / 2) + 1;
      part_a += pair_id;
    }
  }

  const auto nested_2 = bj::nested_ints::read("[[2]]");
  const auto nested_6 = bj::nested_ints::read("[[6]]");

  values.emplace_back(nested_2);
  values.emplace_back(nested_6);
  
  std::sort(values.begin(), values.end(), is_less);
  
  const size_t pos_of_2 = std::find(values.begin(), values.end(), nested_2) - values.begin() + 1;
  const size_t pos_of_6 = std::find(values.begin(), values.end(), nested_6) - values.begin() + 1;

  return Output(part_a, pos_of_2 * pos_of_6);
}
