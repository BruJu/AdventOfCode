#include "../../common/advent_of_code.hpp"


// https://adventofcode.com/2024/day/7

namespace {

enum class SolutionType { False, WithConcat, True };

SolutionType operator||(SolutionType lhs, SolutionType rhs) {
  for (SolutionType val : { SolutionType::True, SolutionType::WithConcat, SolutionType::False }) {
    if (lhs == val || rhs == val) {
      return val;
    }
  }
  return SolutionType::False;
}

std::pair<std::int64_t, std::vector<std::int64_t>> read_line(const std::string & line) {
  const size_t coma_pos = line.find(':');
  if (coma_pos == std::string::npos)
    throw std::runtime_error("2024-07 bad input");
  
  const long long int test_value = std::stoll(line.substr(0, coma_pos));
  const std::string values_str = line.substr(coma_pos + 1);

  return {
    test_value,
    bj::extract_signed_i64_from_line(values_str)
  };
}

std::int64_t get_sup_pow_of_10(std::int64_t value) {
  // fast option
  if (value <=       9) return       10;
  if (value <=      99) return      100;
  if (value <=     999) return     1000;
  if (value <=    9999) return    10000;
  if (value <=   99999) return   100000;
  if (value <=  999999) return  1000000;
  if (value <= 9999999) return 10000000;

  // generic solution
  int64_t pow10 = 10;
  while (pow10 <= value) pow10 *= 10;
  return pow10;
}

std::int64_t concat(std::int64_t lhs, std::int64_t rhs) {
  return lhs * get_sup_pow_of_10(rhs) + rhs;
}

SolutionType can_reach(
  std::int64_t test_value,
  std::int64_t current_value,
  std::span<const std::int64_t> values
  ) {
  if (values.empty()) {
    return test_value == current_value ? SolutionType::True : SolutionType::False;
  }

  const SolutionType withTimes = can_reach(test_value, current_value * values[0], values.subspan(1));
  if (withTimes == SolutionType::True) return SolutionType::True;

  const SolutionType withPlus = can_reach(test_value, current_value + values[0], values.subspan(1));
  if ((withTimes || withPlus) != SolutionType::False) return withTimes || withPlus;

  const SolutionType withConcat = can_reach(test_value, concat(current_value, values[0]), values.subspan(1));
  if (withConcat != SolutionType::False) return SolutionType::WithConcat;

  return SolutionType::False;
}

SolutionType can_reach(std::int64_t test_value, std::span<const std::int64_t> values) {
  if (values.size() == 0) return SolutionType::False;
  return can_reach(test_value, values[0], values.subspan(1));
}

}

Output day_2024_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;
  long long int part_b = 0;

  for (const std::string & line : lines) {
    const auto [test_value, values] = read_line(line);

    const SolutionType solution_type = can_reach(test_value, values);

    if (solution_type == SolutionType::True) {
      part_a += test_value;
      part_b += test_value;
    } else if (solution_type == SolutionType::WithConcat) {
      part_b += test_value;
    }

  }

  return Output(part_a, part_b); 
}