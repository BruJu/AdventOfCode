#include "../../common/advent_of_code.hpp"
#include "./2025.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <map>

// https://adventofcode.com/2025/day/3

namespace {

std::int64_t find_max_voltage(const std::string & line) {
  if (line.size() <= 2) return -1;

  char left = line[0];
  char right = line[1];

  for (size_t i = 1; i < line.size(); ++i) {
    if (line[i] > left && i + 1 < line.size()) {
      left = line[i];
      right = line[i + 1];
    } else if (line[i] > right) {
      right = line[i];
    }
  }

  const int max_joltage = (left - '0') * 10 + (right - '0');
  return max_joltage;
}

template<size_t N>
std::int64_t find_max_voltage_n(const std::string & line) {
  if (line.size() < N) {
    return -1;
  }

  // Initialize to first N digits
  std::array<char, N> max_digits;

  for (size_t i = 0; i != N; ++i) {
    max_digits[i] = line[i];
  }

  // Explore next digits
  for (size_t i = N; i < line.size(); ++i) {
    // Virtually, we build the array [...max_digits, line[i]]
    // And to go back to N character, we want to remove the first digit such that
    // it is less than the next one.
    size_t digit_to_remove = 0;
    for (; digit_to_remove < N; ++digit_to_remove) {
      int compare_value;
      if (digit_to_remove + 1 == N) {
        compare_value = line[i];
      } else {
        compare_value = max_digits[digit_to_remove + 1];
      }

      if (max_digits[digit_to_remove] < compare_value) {
        break;
      }
    }

    if (digit_to_remove == N) continue;

    for (size_t i = digit_to_remove; i + 1 < N; ++i) {
      max_digits[i] = max_digits[i + 1];
    }

    max_digits[N - 1] = line[i];
  }
  

  // Finalize
  std::int64_t result = 0;
  for (char c : max_digits) {
    result = result * 10 + (c - '0');
  }

  return result;
}

}

Output day_2025_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  return aoc_is_a_function(lines, find_max_voltage, find_max_voltage_n<12>);
}
