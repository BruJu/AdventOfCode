#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/12

// Approach:
// Store the string and the list of group of # that are searched
// String has no leading .
// We start at the first position of the string and at the 0th group
// - If it is a #, make sure there are the required number of # or ?, then find a . or #, and repeat
// - If it is a ?, find next # or ?, and add the result of this operation to the result of the above

struct MemoizedCountArrangement {
private:
  std::string spring;
  std::span<const int> record;
  std::map<std::pair<size_t, size_t>, i64> nb_of_arangements;

public:
  MemoizedCountArrangement(std::string spring, std::span<const int> record)
  : spring(spring), record(record) {}

  [[nodiscard]] i64 evaluate(size_t i_spring, size_t i_record) {
    const auto key = std::pair(i_spring, i_record);

    const auto it = nb_of_arangements.find(key);
    if (it != nb_of_arangements.end()) {
      return it->second;
    }

    i64 eval = actually_evaluate(i_spring, i_record);
    nb_of_arangements.emplace(key, eval);
    return eval;
  }

private:
  [[nodiscard]] i64 actually_evaluate(size_t i_spring, size_t i_record) {
    // No more set of damaged to find = reach the end
    if (record.size() == i_record) {
      while (i_spring != spring.size()) {
        if (spring[i_spring] == '#') {
          return 0;
        }
        ++i_spring;
      }

      return 1;
    }

    // End of spring but still has record = nope
    if (i_spring == spring.size()) return 0;

    i64 sum = 0;
    if (spring[i_spring] == '?') {
      // Pretend it is not damaged
      sum += evaluate(skip_to_next(i_spring + 1), i_record);
    }

    // It is damaged
    for (int i = 0; i < record[i_record]; ++i) {
      if (i_spring == spring.size() || spring[i_spring] == '.') {
        return sum;
      }

      ++i_spring;
    }

    // Now we need a separator

    // - Ok, end of string
    if (spring.size() == i_spring) return sum + evaluate(i_spring, i_record + 1);

    // - Ok found non #
    if (spring[i_spring] != '#') {
      return sum + evaluate(skip_to_next(i_spring + 1), i_record + 1);
    }

    // Not ok
    return sum;
  }

  size_t skip_to_next(size_t i) const {
    while (i < spring.size() && spring[i] == '.') {
      ++i;
    }
    return i;
  }
};


i64 evaluate(std::string spring, std::span<const int> record) {
  MemoizedCountArrangement memoized { spring, record };
  return memoized.evaluate(0, 0);
}

std::string bump(const std::string & s) {
  std::string res = s;
  for (size_t i = 0; i != 4; ++i) {
    res += '?';
    res += s;
  }
  return res;
}

std::vector<int> bump(std::span<const int> vals) {
  std::vector<int> res;
  for (size_t i = 0; i != 5; ++i) {
    for (int x : vals) {
      res.push_back(x);
    }
  }

  return res;
}

std::string trim_dots(const std::string & original_word) {
  size_t i = 0;
  while (i < original_word.size() && original_word[i] == '.') {
    ++i;
  }
  return original_word.substr(i);
}


Output day_2023_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  i64 part_a = 0;
  i64 part_b = 0;

  for (const std::string & line : lines) {
    const size_t space = line.find(' ');
    const std::string spring = line.substr(0, space);
    const std::string record_str = line.substr(space + 1);
    const std::vector<int> records = bj::extract_ints_from_line(record_str);

    part_a += evaluate(trim_dots(spring), records);

    const std::string spring_b = bump(spring);
    const std::vector<int> records_b = bump(records);

    part_b += evaluate(trim_dots(spring_b), records_b);
  }


  return Output(part_a, part_b);
}
