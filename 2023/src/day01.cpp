#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>

// https://adventofcode.com/2023/day/1

// Read the first and the last digit in a string
static int get_calibration_simple(const std::string & line) {
  int first = -1;
  int last = 0;

  for (const char c : line) {
    if (c < '0' || c > '9') continue;

    const int value = c - '0';
    if (first == -1) first = value;
    last = value;
  }

  return first * 10 + last;
}

// Read the first and the last digit in a string
// Except they can be written in plain letters
// and twone is 21.

int find_first(const std::string & line, std::span<const std::pair<int, std::string>> values) {
  std::optional<size_t> first_position;
  int result = 0;

  for (const auto & [digit_int, digit_str] : values) {
    const size_t pos = line.find(digit_str);

    if (pos == std::string::npos) continue;
    if (first_position && first_position.value() < pos) continue;

    first_position = pos;
    result = digit_int;
  }

  return result;
}

using Digits2Digits = std::vector<std::pair<int, std::string>>;

static Digits2Digits get_list_of_digits() {
  std::vector<std::pair<int, std::string>> existing_digits;
  existing_digits.emplace_back(0, "0");
  existing_digits.emplace_back(1, "1");
  existing_digits.emplace_back(1, "one");
  existing_digits.emplace_back(2, "2");
  existing_digits.emplace_back(2, "two");
  existing_digits.emplace_back(3, "3");
  existing_digits.emplace_back(3, "three");
  existing_digits.emplace_back(4, "4");
  existing_digits.emplace_back(4, "four");
  existing_digits.emplace_back(5, "5");
  existing_digits.emplace_back(5, "five");
  existing_digits.emplace_back(6, "6");
  existing_digits.emplace_back(6, "six");
  existing_digits.emplace_back(7, "7");
  existing_digits.emplace_back(7, "seven");
  existing_digits.emplace_back(8, "8");
  existing_digits.emplace_back(8, "eight");
  existing_digits.emplace_back(9, "9");
  existing_digits.emplace_back(9, "nine");
  return existing_digits;
}

static Digits2Digits get_reverse_list_of_digits() {
  Digits2Digits existing_digits = get_list_of_digits();

  for (auto & [_, string_representation] : existing_digits) {
    std::reverse(string_representation.begin(), string_representation.end());
  }

  return existing_digits;
}

int get_calibration_complicated(std::string line, const Digits2Digits & good_order, const Digits2Digits & reverse_order) {
  // Find first
  const int first_digit = find_first(line, good_order);

  // Find last
  std::reverse(line.begin(), line.end());
  const int second_digit = find_first(line, reverse_order);

  return first_digit * 10 + second_digit;
}


Output day_2023_01(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  int part_a = 0;
  int part_b = 0;

  if (!dei.can_skip_part_A) {
    for (const std::string & line : lines) {
      part_a += get_calibration_simple(line);
    }
  }

  if (!dei.can_skip_part_B) {
    const Digits2Digits list_of_digits = get_list_of_digits();
    const Digits2Digits list_of_reverse_digits = get_reverse_list_of_digits();

    for (const std::string & line : lines) {
      part_b += get_calibration_complicated(line, list_of_digits, list_of_reverse_digits);
    }
  }

  return Output(part_a, part_b);
}
