#include "../advent_of_code.hpp"
#include <map>

// https://adventofcode.com/2018/day/02


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
  std::string part_b_answer;
  for (size_t i = 0; i + 1 < lines.size(); ++i) {
    const std::string & lhs = lines[i];
    const std::string & rhs = lines[i + 1];

    if (lhs.size() != rhs.size()) continue;

    size_t letter_diff_position = std::string::npos;
    size_t p = 0;
    for (; p != lhs.size(); ++p) {
      if (lhs[p] != rhs[p]) {
        if (letter_diff_position == std::string::npos) {
          letter_diff_position = p;
        } else {
          break;
        }
      }
    }

    if (p == lhs.size()) {
      std::cout << lhs << " == " << rhs << " : " << letter_diff_position << "\n";
      part_b_answer = lhs;
      if (letter_diff_position != std::string::npos) {
        part_b_answer = part_b_answer.substr(0, letter_diff_position) + part_b_answer.substr(letter_diff_position + 1);
      }

      break;
    }
  }

  return Output(number_twice * number_thrice, part_b_answer);
}
