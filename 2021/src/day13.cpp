#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <cstring>
#include "../../util/position.hpp"

// https://adventofcode.com/2021/day/13

///////////////////////////////////////////////////////////////////////////////

enum class Axis { x, y };

struct FoldInstruction {
  Axis axis;
  int line;
};

/** Transforms the given list of strings into a list of dots */
static std::set<bj::Position> read_initial_dots(
  const std::vector<std::string>::const_iterator begin,
  const std::vector<std::string>::const_iterator end
);

/** Transform the givne list of strings into a list of folding instructions */
static std::vector<FoldInstruction> read_folding_instructions(
  const std::vector<std::string>::const_iterator begin,
  const std::vector<std::string>::const_iterator end
);

/** Transforms all points according to a folding instruction */
static std::set<bj::Position> fold(
  const std::set<bj::Position> & original,
  FoldInstruction instruction
);

/** Transform a point according to a folding instruction */
static bj::Position transform_point(bj::Position point, FoldInstruction instruction);

/** Compute the position of the given position considering it is reflecting if >= line */
static int reflect(int line, int pos);

/** Draw on std::cout the dots */
static void draw(const std::set<bj::Position> & dots);


///////////////////////////////////////////////////////////////////////////////

Output day_2021_13(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const auto empty_line = std::find(lines.begin(), lines.end(), "");
  if (empty_line == lines.end()) {
    std::cerr << "No empty line found in input\n";
    return Output(0, 0);
  }

  std::set<bj::Position> dots = read_initial_dots(lines.begin(), empty_line);
  std::vector<FoldInstruction> instructions = read_folding_instructions(empty_line + 1, lines.end());

  dots = fold(dots, instructions[0]);

  long long int part_a = dots.size();

  for (size_t i = 1; i != instructions.size(); ++i) {
    dots = fold(dots, instructions[i]);
  }

  if (!dei.can_skip_part_B) {
    draw(dots);
  }

  return Output(part_a, 0);
}


///////////////////////////////////////////////////////////////////////////////
// Cool folding

static std::set<bj::Position> fold(
  const std::set<bj::Position> & original,
  FoldInstruction instruction
) {
  std::set<bj::Position> new_paper;

  for (const auto point : original) {
    new_paper.emplace(transform_point(point, instruction));
  }

  return new_paper;
}

static bj::Position transform_point(bj::Position point, FoldInstruction instruction) {
  if (instruction.axis == Axis::y) {
    return bj::Position { point.x, reflect(instruction.line, point.y) };
  } else {
    return bj::Position { reflect(instruction.line, point.x), point.y };
  }
}

static int reflect(int line, int pos) {
  if (pos < line) return pos;
  const auto overflow = pos - line;
  return line - overflow;
}


///////////////////////////////////////////////////////////////////////////////
// Boring I/O

static std::set<bj::Position> read_initial_dots(
  const std::vector<std::string>::const_iterator begin,
  const std::vector<std::string>::const_iterator end
) {
  std::set<bj::Position> dots;

  for (auto it = begin; it != end; ++it) {
    const auto coma_pos = it->find(',');
    if (coma_pos == std::string::npos) {
      std::cerr << "no , in string" << *it << '\n';
      continue;
    }

    const int x = std::stoi(it->substr(0, coma_pos));
    const int y = std::stoi(it->substr(coma_pos + 1));
    dots.emplace(bj::Position{ x, y });
  }

  return dots;
}

static std::vector<FoldInstruction> read_folding_instructions(
  const std::vector<std::string>::const_iterator begin,
  const std::vector<std::string>::const_iterator end
) {
  std::vector<FoldInstruction> instructions;
  
  const auto prefix_length = std::strlen("fold along x=");

  for (auto it = begin; it != end; ++it) {
    if (*it == "") continue;

    Axis axis;
    if (it->starts_with("fold along x=")) {
      axis = Axis::x;
    } else {
      axis = Axis::y;
    }

    instructions.push_back(FoldInstruction{
      axis,
      std::stoi(it->substr(prefix_length))
    });
  }

  return instructions;
}

static void draw(const std::set<bj::Position> & dots) {
  int left = 0;
  int right = 0;
  int top = 0;
  int bottom = 0;

  for (const auto & [x, y] : dots) {
    if (x < left) left = x;
    if (x > right) right = x;
    if (y < top) top = y;
    if (y > bottom) bottom = y;
  }

  for (int y = top; y <= bottom; ++y) {
    for (int x = left; x <= right; ++x) {
      if (dots.contains(bj::Position{ x, y })) {
        std::cout << '#';
      } else {
        std::cout << ' ';
      }
    }
    std::cout << '\n';
  }
}
