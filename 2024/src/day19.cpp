#include "../../common/advent_of_code.hpp"
#include <string_view>
#include <set>
#include <functional>

// https://adventofcode.com/2024/day/19

using i64 = std::int64_t;
using Patterns = std::set<std::string, std::less<>>;

namespace {

Patterns read_patterns(const std::string & first_line) {
  Patterns out;
  std::string buffer;

  for (char c : first_line) {
    if (c == ',') {
      out.emplace(buffer);
      buffer = "";
    } else if (c == ' ') {

    } else {
      buffer += c;
    }
  }

  if (buffer != "") out.emplace(buffer);

  return out;
}

size_t size_of_longest_pattern(const Patterns & patterns) {
  if (patterns.empty()) return 0;

  size_t s = 0;
  for (const std::string & str : patterns) {
    s = std::max(s, str.size());
  }
  return s;
}

i64 is_legit_towel(std::string_view design, const std::set<std::string, std::less<>> & patterns) {
  const size_t longest_pattern = size_of_longest_pattern(patterns);

  // Dynamic programming from the end
  std::vector<i64> nb_ways_to_reach;
  nb_ways_to_reach.resize(design.size() + 1, 0);

  nb_ways_to_reach[design.size()] = 1; // One way to build an empty design: do nothing

  for (int i = static_cast<int>(design.size()) - 1; i >= 0; --i) {
    // Considering the design design[i:]
    for (size_t j = 1; j <= longest_pattern; ++j) {
      if (i + j > design.size()) {
        continue;
      }

      // Does towel design[i:j] exist?
      std::string_view towel = design.substr(i, j);      
      if (patterns.contains(towel)) {
        // If yes, we discovered a number of way to build design[i:] equal to the number of ways to build design[j:]:
        // by first placing the towel design[i:j] and then using all the methods to build design[j:] discovered
        // on previous iterations
        nb_ways_to_reach[i] += nb_ways_to_reach[i + j];
      }
    }
  }

  // Don't forget to bring a towel 
  return nb_ways_to_reach[0];
}

}

Output day_2024_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const Patterns patterns = read_patterns(lines[0]);

  i64 nb_possible_designs = 0;
  i64 nb_arrangements     = 0;
  for (size_t i = 2; i != lines.size(); ++i) {
    const i64 nb_ways = is_legit_towel(lines[i], patterns);

    if (nb_ways > 0) nb_possible_designs += 1;
    nb_arrangements += nb_ways;
  }
  
  return Output(nb_possible_designs, nb_arrangements);
}
