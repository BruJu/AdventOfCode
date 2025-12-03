#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <set>
#include <map>
#include <numeric>

// https://adventofcode.com/2025/day/2

namespace {

int count_number_of_digits(std::int64_t number) {
  if (number == 0) return 1;

  int digits = 0;
  while (number > 0) {
    number /= 10;
    ++digits;
  }
  
  return digits;
}

// compute_power_of_10(n) == 10^n
std::int64_t compute_power_of_10(std::int64_t exponent) {
  std::int64_t result = 1;
  for (int i = 0; i < exponent; ++i) {
    result *= 10;
  }
  
  return result;
}

std::int64_t compute_x_number(std::int64_t number, unsigned int x) {
  const std::int64_t number_of_digits = count_number_of_digits(number);
  const std::int64_t power_of_10 = compute_power_of_10(number_of_digits);

  std::int64_t x_number = 0;
  for (unsigned int i = 0; i < x; ++i) {
    x_number = x_number * power_of_10 + number;
  }
  return x_number;
}

std::int64_t find_smallest_gte_x_number(std::int64_t number, unsigned int x) {
  std::int64_t min = 0;

  while (true) {
    std::int64_t d_number_min = compute_x_number(min, x);
    if (d_number_min < number) {
      ++min;
    } else {
      return min;
    }
  }
}

std::vector<std::pair<std::int64_t, std::int64_t>> boring_input_computing(std::span<const std::string> lines) {
  // Remove line breaks
  std::string single_line = "";
  for (const std::string & line : lines) {
    single_line += line;
  }

  // Make extract_signed_i64_from_line by replacing separator with another separator
  for (char & c : single_line) {
    if (c == '-') {
      c = ' ';
    }
  }

  // Extract numbers
  std::vector<std::int64_t> values = bj::extract_signed_i64_from_line(single_line);

  // Make pairs
  if (values.size() % 2 != 0) {
    throw std::runtime_error("2025-02 expects an ever number of integers");
  }

  std::vector<std::pair<std::int64_t, std::int64_t>> bounds;
  for (size_t i = 0; i != values.size(); i += 2) {
    bounds.emplace_back(values[i], values[i + 1]);
  }

  return bounds;
}

void
find_all_invalid_ids(
  std::set<std::int64_t> & output,
  std::int64_t left, std::int64_t right, std::int64_t number_of_repeats
) {
  std::int64_t start_x_number_id = find_smallest_gte_x_number(left, number_of_repeats);

  while (true) {
    std::int64_t x_value = compute_x_number(start_x_number_id, number_of_repeats);
    if (x_value > right) {
      break;
    }

    output.emplace(x_value);
    ++start_x_number_id;
  }
}

}



Output day_2025_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<std::pair<std::int64_t, std::int64_t>> bounds = boring_input_computing(lines);

  std::int64_t only_2_numbers = 0;
  std::int64_t all_numbers = 0;

  for (const auto & [left, right] : bounds) {
    std::set<int64_t> values;

    // Part A
    find_all_invalid_ids(values, left, right, 2);   
    only_2_numbers += std::reduce(values.begin(), values.end());

    // Part B
    const auto digits = count_number_of_digits(right);

    for (int x = 3; x <= digits; ++x) {
      find_all_invalid_ids(values, left, right, x);
    }

    all_numbers += std::reduce(values.begin(), values.end());
  }
  
  return Output(only_2_numbers, all_numbers);
}
