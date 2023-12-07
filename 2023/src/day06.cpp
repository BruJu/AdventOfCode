#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>
#include <map>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <exception>
#include <cmath>

// https://adventofcode.com/2023/day/6

using i64 = long long int;

struct how_many_ways_to_win {

// Ok for part_a, not for part_b
static i64 loop(i64 time, i64 distance) {
  i64 count = 0;

  for (i64 x = 1; x <= time; ++x) {
    if ((time - x) * x > distance) {
      ++count;
    }
  }

  return count;
}

// Ok for both
static i64 maths(i64 time, i64 distance) {
  // f(x) = -x² + time * x - distance > 0

  double delta = sqrt(time * time - 4 * (-1) * (-distance));
  
  // Find roots
  double upper = (-time - delta) / (2. * -1);
  double lower = (-time + delta) / (2. * -1);

  // f(x) >= 0
  i64 lower_i = ceil(lower);
  i64 higher_i = floor(upper);

  // f(x) > 0
  if (floor(lower) == lower) ++lower_i;
  if (floor(upper) == upper) --higher_i;

  // assert(lower_i <= higher_i);
  return higher_i - lower_i + 1;
}

};

long long int read_int(const std::string & str) {
  long long int r = 0;

  for (const char c : str) {
    if (c >= '0' && c <= '9') {
      r = r * 10 + (c - '0');
    }
  }

  return r;
}

Output day_2023_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<int> times = bj::extract_ints_from_line(lines[0]);
  const std::vector<int> distances = bj::extract_ints_from_line(lines[1]);

  i64 part_a = 1;
  for (size_t i = 0; i != times.size(); ++i) {
    const int time = times[i];
    const int distance = distances[i];

    const i64 valid_xs = how_many_ways_to_win::maths(time, distance);
    
    part_a *= valid_xs;
  }
  
  const i64 time_b = read_int(lines[0]);
  const i64 distance_b = read_int(lines[1]);

  i64 part_b = how_many_ways_to_win::maths(time_b, distance_b);

  return Output(part_a, part_b);
}
