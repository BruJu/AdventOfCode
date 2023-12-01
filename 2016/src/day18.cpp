#include "../../common/advent_of_code.hpp"
#include <algorithm>

static char get_symbol(bool left_trap, bool mid_trap, bool right_trap) {
  if ((left_trap && mid_trap && !right_trap)
    || (!left_trap && mid_trap && right_trap)
    || (left_trap && !mid_trap && !right_trap)
    || (!left_trap && !mid_trap && right_trap)) {
    return '^';
  } else {
    return '.';
  }
}

static std::string build_new_line(const std::string & previous) {
  std::string new_line;

  for (size_t i = 0; i != previous.size(); ++i) {
    new_line += get_symbol(
      i == 0 ? false : previous[i - 1] == '^',
      previous[i] == '^',
      i + 1 == previous.size() ? false : previous[i + 1] == '^'
    );
  }

  return new_line;
}

static size_t count(const std::vector<std::string> & grid) {
  size_t r = 0;

  for (const auto & line : grid) {
    r += std::count(line.begin(), line.end(), '.');
  }

  return r;
}

void transform(std::string & dest, const std::string & src) {
  for (size_t i = 0; i != src.size(); ++i) {
    dest[i] = get_symbol(
      i == 0 ? false : src[i - 1] == '^',
      src[i] == '^',
      i + 1 == src.size() ? false : src[i + 1] == '^'
    );
  }
}


Output day_2016_18(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<std::string> grid;
  grid.push_back(lines[0]);

  while (grid.size() != std::stoul(lines[1])) {
    grid.push_back(build_new_line(grid.back()));
  }

  const size_t part_a = count(grid);

  size_t part_b = part_a;

  if (!dei.can_skip_part_B) {
    // Keeping the grid is actually useless -> we just want to know the number
    // of safe spots

    // Pre allocate two big enough strings
    std::string even = grid.back(); // even is line 40
    std::string odd  = grid.back();

    size_t count = std::stoul(lines[1]);

    while (count != 400000) {
      transform(odd, even);   // compute line even + 1 in odd
      transform(even, odd);   // compute line even + 2 in even

      // count
      part_b += std::count(odd.begin(), odd.end(), '.');
      part_b += std::count(even.begin(), even.end(), '.');

      // we just did two lines
      count += 2;
    }
  }

  return Output(part_a, part_b);
}
