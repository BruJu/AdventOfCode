#include "../advent_of_code.hpp"
#include <set>

// https://adventofcode.com/2017/day/04

static bool has_no_duplicate(const std::string & line) {
  const auto words = bj::string_split(line, " ");

  std::set<std::string> unique_words;
  for (const auto & word : words) {
    if (unique_words.contains(word)) return false;
    unique_words.emplace(word);
  }

  return true;
}

static bool has_no_anagram(const std::string & line) {
  const auto words = bj::string_split(line, " ");

  std::set<std::string> unique_words;
  for (std::string word : words) {
    std::sort(word.begin(), word.end());
    if (unique_words.contains(word)) return false;
    unique_words.emplace(word);
  }

  return true;
}

Output day_2017_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  size_t no_duplicate = 0;
  size_t no_anagram = 0;

  for (const std::string & line : lines) {
    if (has_no_duplicate(line)) { ++no_duplicate; }
    if (has_no_anagram(line)) { ++no_anagram; }
  }

  return Output(no_duplicate, no_anagram);
}
