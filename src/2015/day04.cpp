#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include "../util/md5.h"

// https://adventofcode.com/2015/day/4

static long long int find_answer(size_t number_of_zeros, const std::string & input) {
  const auto is_answer = [&](const std::string & candidate) {
    for (size_t i = 0; i != number_of_zeros; ++i) {
      if (candidate[i] != '0') return false;
    }
    return true;
  };

  MD5 md5;
  long long int x = 0;

  while (true) {
    const auto res = md5(input + std::to_string(x));

    if (is_answer(res)) {
      return x;
    }

    ++x;
  }
}

Output day_2015_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  return Output(
    find_answer(5, lines[0]),
    find_answer(6, lines[0])
  );
}
