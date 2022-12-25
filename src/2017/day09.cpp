#include "../advent_of_code.hpp"

// https://adventofcode.com/2017/day/09


Output day_2017_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string line = lines[0];
  
  int score = 0;
  int current_nest = 0;

  int ignored = 0;

  size_t i = 0;
  bool is_in_garbage = false;
  bool ignore_next = false;
  while (i < line.size()) {
    if (!is_in_garbage) {
      if (line[i] == '{') {
        ++current_nest;
        score += current_nest;
      } else if (line[i] == '}') {
        --current_nest;
      } else if (line[i] == '<') {
        is_in_garbage = true;
        ignore_next = false;
      } else if (line[i] == ',') {
        // noop
      } else {
        std::cerr << line[i] << "\n";
        throw std::runtime_error("unexpected symbol out of garbage");
      }
    } else {
      if (ignore_next) {
        ignore_next = false;
      } else {
        if (line[i] == '>') {
          is_in_garbage = false;
        } else if (line[i] == '!') {
          ignore_next = true;
        } else {
          ++ignored;
        }
      }
    }

    ++i;
  }

  return Output(score, ignored);
}
