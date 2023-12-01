#include "../../common/advent_of_code.hpp"
#include <set>
#include <string>

// https://adventofcode.com/2018/day/05

static bool is_capitalized(char c) {
  return c >= 'A' && c <= 'Z';
};

static char normalize(char c) {
  if (is_capitalized(c)) {
    return c;
  } else {
    return static_cast<char>(c - 'a' + 'A');
  }
};

static bool is_same_type(char a, char b) {
  return normalize(a) == normalize(b);
}

static std::string react(std::string polymer) {
  while (true) {
    bool stable = true;

    size_t i = 0;
    while (i + 1 < polymer.size()) {
      char left = polymer[i];
      char right = polymer[i + 1];

      if (left != right && is_same_type(left, right)) {
        polymer.erase(i, 2);
        stable = false;
        if (i != 0) --i;
      } else {
        ++i;
      }
    }

    if (stable) break;
  }

  return polymer;
}

static std::string remove_all_in_polymer(std::string polymer, char to_remove) {
  polymer.erase(
    std::remove_if(
      polymer.begin(), polymer.end(),
      [&](char in_polymer) {
        return normalize(to_remove) == normalize(in_polymer);
      }
    ),
    polymer.end()
  );

  return polymer;
}

Output day_2018_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string polymer = lines[0];

  const std::string part_a = react(polymer);

  std::set<char> types;
  for (const char letter : polymer) {
    types.emplace(normalize(letter));
  }

  std::optional<size_t> shorter_polymer_size = std::nullopt;

  for (const char letter : types) {
    std::string new_polymer = remove_all_in_polymer(polymer, letter);
    new_polymer = react(new_polymer);

    if (!shorter_polymer_size || new_polymer.size() < shorter_polymer_size.value()) {
      shorter_polymer_size = new_polymer.size();
    }
  }

  return Output(part_a.size(), shorter_polymer_size.value_or(0));
}
