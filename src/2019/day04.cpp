#include "../advent_of_code.hpp"
#include <string>

// https://adventofcode.com/2019/day/04

static bool is_password(int key) {
  if (key < 100000 || key > 999999) return false;

  bool double_digit = false;

  int last_digit = 10;

  while (key != 0) {
    const int digit = key % 10;
    key /= 10;

    if (digit > last_digit) {
      return false;
    } else if (digit == last_digit) {
      double_digit = true;
    } else {
      last_digit = digit;
    }
  }

  return double_digit;
}

static bool is_stronger(std::string key) {
  for (size_t i = 0; i != 5; ++i) {
    const bool same = key[i] == key[i + 1];
    const bool not_before = i == 0 || key[i - 1] != key[i];
    const bool not_after = i == 4 || key[i + 1] != key[i + 2];
    if (same && not_before && not_after) {
      return true;
    }
  }
  return false;
}

Output day_2019_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::string line = lines[0];

  int min;
  int max;
  const auto minus = line.find("-");
  if (minus == std::string::npos) {
    min = std::stoi(line);
    max = std::stoi(line);
  } else {
    min = std::stoi(line.substr(0, minus));
    max = std::stoi(line.substr(minus + 1));
  }

  size_t pwds = 0;
  size_t stronger_pwds = 0;
  for (int i = min; i <= max; ++i) {
    if (is_password(i)) {
      ++pwds;

      if (is_stronger(std::to_string(i))) {
        ++stronger_pwds;
      }
    }
  }

  return Output(pwds, stronger_pwds);
}


// <=2060945012