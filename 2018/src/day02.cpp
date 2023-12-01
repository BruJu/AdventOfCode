#include "../../common/advent_of_code.hpp"
#include <map>
#include <span>

// https://adventofcode.com/2018/day/02


namespace {

std::optional<std::string> compare(const std::string & lhs, const std::string & rhs) {
  if (lhs.size() != rhs.size()) return std::nullopt;

  size_t letter_diff_position = std::string::npos;
  size_t p = 0;
  for (; p != lhs.size(); ++p) {
    if (lhs[p] != rhs[p]) {
      if (letter_diff_position == std::string::npos) {
        letter_diff_position = p;
      } else {
        return std::nullopt;
      }
    }
  }

  if (p != lhs.size()) return std::nullopt;

  std::string retval = lhs;
  if (letter_diff_position != std::string::npos) {
    retval.erase(letter_diff_position, 1);
  }
  return retval;
}

std::optional<std::string> part_b(std::span<const std::string> boxes) {
  for (size_t i = 0; i != boxes.size(); ++i) {
    for (size_t j = i + 1; j < boxes.size(); ++j) {
      const auto maybe = compare(boxes[i], boxes[j]);
      if (maybe) return maybe;
    }
  }
  return std::nullopt;
}

}

Output day_2018_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Part A
  size_t number_twice = 0;
  size_t number_thrice = 0;

  for (const std::string & line : lines) {
    std::map<char, size_t> letter_occurrences;

    for (const char c : line) {
      ++letter_occurrences[c];
    }

    bool has_twice = false;
    bool has_thrice = false;
    for (const auto & [_symbol, number] : letter_occurrences) {
      if (number == 2) has_twice = true;
      if (number == 3) has_thrice = true;
    }

    if (has_twice) ++number_twice;
    if (has_thrice) ++number_thrice;
  }
  
  // Part B
  const auto part_b_answer = part_b(lines).value_or("No value");

  return Output(number_twice * number_thrice, part_b_answer);
}
