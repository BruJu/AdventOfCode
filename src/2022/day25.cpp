#include "../advent_of_code.hpp"

// https://adventofcode.com/2022/day/25

static std::uint64_t snafu_to_int(const std::string & snafu_number) {
  std::uint64_t res = 0;

  for (char c : snafu_number) {
    switch (c) {
      case '=': res = res * 5 - 2; break;
      case '-': res = res * 5 - 1; break;
      case '0': res = res * 5 + 0; break;
      case '1': res = res * 5 + 1; break;
      case '2': res = res * 5 + 2; break;
    }
  }

  return res;
}

static std::string int_to_snafu(std::uint64_t int_number) {
  std::string buffer;

  while (int_number > 0) {
    const std::uint64_t display = int_number % 5;
    if (display >= 3) int_number += 5; // Hold
    int_number /= 5;

    switch (display) {
      case 0: buffer += '0'; break;
      case 1: buffer += '1'; break;
      case 2: buffer += '2'; break;
      case 3: buffer += '='; break;
      case 4: buffer += '-'; break;
    }
  }

  return std::string(buffer.rbegin(), buffer.rend());
}

Output day_2022_25(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::uint64_t sum = 0;

  for (const std::string & snafu_number : lines) {
    sum += snafu_to_int(snafu_number);
  }

  return Output(int_to_snafu(sum), "");
}
