#include "../advent_of_code.hpp"
#include <vector>
#include <set>
#include "../libs_ensemblist.hpp"

// https://adventofcode.com/2022/day/3

// a-z = 1-26 / A-Z = 27-52
static int priority(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 27;
  } else {
    return c - 'a' + 1;
  }
}

Output day_2022_03(const std::vector<std::string> & lines, const DayExtraInfo &) {

  // Part A: A line cut the line and find the common item
  long long int part_a = 0;
  for (const std::string & line : lines) {
    // A string -> cut in two equals parts
    const std::string first = line.substr(0, line.size() / 2);
    const std::string second = line.substr(line.size() / 2);

    const std::set<char> left(first.begin(), first.end());
    const std::set<char> right(second.begin(), second.end());

    // Find common items between the two parts
    const std::set<char> intersection = set::intersection(left, right);

    // Add the priority of the letter to the answer
    for (const char c : intersection) {
      part_a += priority(c);
    }
  }

  // Part B: Group the lines / elves by 3, and find the common item
  long long int part_b = 0;
  for (size_t i = 0; i < lines.size(); i = i + 3) {
    // Who is in the group?
    const std::set<char> elve1(lines[i+0].begin(), lines[i+0].end());
    const std::set<char> elve2(lines[i+1].begin(), lines[i+1].end());
    const std::set<char> elve3(lines[i+2].begin(), lines[i+2].end());

    // Big intersection
    const std::set<char> inter = set::intersection(set::intersection(elve1, elve2), elve3);

    // Add priority of the common item
    if (inter.size() == 1) {
      part_b += priority(*inter.begin());
    } else {
      std::cout << "A group of elves has no unique common item\n";
    }
  }

  return Output(part_a, part_b);
}
