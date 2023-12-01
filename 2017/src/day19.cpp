#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <vector>
#include <map>
#include <cstring>
#include <array>
#include <variant>
#include <deque>

// https://adventofcode.com/2017/day/19


void turn_left(bj::Direction & direction) {
  switch (direction) {
    case bj::Direction::Left: direction = bj::Direction::Down; return;
    case bj::Direction::Down: direction = bj::Direction::Right; return;
    case bj::Direction::Right: direction = bj::Direction::Top; return;
    case bj::Direction::Top: direction = bj::Direction::Left; return;
  }
}




Output day_2017_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<std::string> grid = lines;
  if (grid.back().size() != grid.front().size()) {
    grid.back().resize(grid.front().size(), ' ');
  }

  int x = 0;
  while (std::cmp_less(x, lines.front().size()) && grid[0][x] != '|') {
    ++x;
  }

  if (x == lines.front().size()) {
    throw std::runtime_error("| is expected to be on first line");
  }

  bj::Position pos { x, 0 };
  bj::Direction dir = bj::Direction::Down;

  std::string encountered_letters;
  long long int steps = 0;
  while (true) {
    pos.move(dir);
    ++steps;

    char c = lines[pos.y][pos.x];

    if (c >= 'A' && c <= 'Z') {
      encountered_letters += c;
    } else if (c == ' ') {
      break;
    } else if (c == '+') {
      turn_left(dir);

      bj::Position next = pos;
      next.move(dir);
      if (lines[next.y][next.x] == ' ') {
        turn_left(dir);
        turn_left(dir);
      }
    }
  }

  return Output(encountered_letters, steps);
}
