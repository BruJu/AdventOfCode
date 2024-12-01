#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <map>

// https://adventofcode.com/2024/day/1

struct LineReader {
  std::regex pattern { R"(^(\d+)\s+(\d+)$)" } ;
  std::smatch matches;

  [[nodiscard]] std::pair<int, int> operator()(const std::string & line) {
    if (!std::regex_search(line, matches, pattern))
      throw std::runtime_error("2024-01 unknown line " + line);
    const int a = std::stoi(matches[1].str());
    const int b = std::stoi(matches[2].str());
    return { a, b };
  }
};

Output day_2024_01(const std::vector<std::string> & lines, const DayExtraInfo &) {
  int part_a = 0;
  long long int part_b = 0;

  std::vector<int> list1;
  std::vector<int> list2;

  list1.reserve(lines.size());
  list2.reserve(lines.size());

  // Read input (two list of numbers with the same size, displayed in columns)
  LineReader line_reader;

  for (const auto & line : lines) { // O(n)
    const auto [a, b] = line_reader(line);
    list1.emplace_back(a);
    list2.emplace_back(b);
  }

  // Part 1
  // - Sort then compute distance
  {
    std::ranges::sort(list1); // O(n logn)
    std::ranges::sort(list2);

    // O(n)
    for (const auto & [a, b] : std::views::zip(list1, list2)) {
      const int distance = std::abs(a - b);
      part_a += distance;
    }
  }

  // Part 2
  // - For each number n in list1, compute n * number of occurrences of n in list2
  // The ordering of the lists changed but it is mostly irrelevant
  // (maybe a inserting a sorted list is bad for std::map perf)
  {
    // O(nlogn)
    std::map<int, int> list2Occurrences;
    for (const int rightValue : list2) {
      list2Occurrences[rightValue]++;
    }

    // O(nlogn)
    for (const int leftValue : list1) {
      part_b += leftValue * list2Occurrences[leftValue];
    }
  }

  // Time complexity: O(nlogn)

  // Ideas for improvment:
  // - Use a heap for part 1 (but does not save any time complexity)
  // - Use two maps from the beginning (saves times if the same numbers occurs a lot of time)

  return Output(part_a, part_b);
}
