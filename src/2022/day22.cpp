#include "../advent_of_code.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include "../util/position.hpp"
// https://adventofcode.com/2022/day/22


struct Grid;

struct Status {
  bj::Position where;
  bj::Direction towards;

  void turn_left();
  void turn_right();
  void move(const Grid & grid, int steps);
  void move(const Grid & grid);
};

void Status::move(const Grid & grid, int steps) {
  for (int i = 0; i != steps; ++i) {
    move(grid);
  }
}

void Status::turn_right() {
  switch (towards) {
    case bj::Direction::Down: towards = bj::Direction::Left; return;
    case bj::Direction::Left: towards = bj::Direction::Top; return;
    case bj::Direction::Top : towards = bj::Direction::Right; return;
    case bj::Direction::Right: towards = bj::Direction::Down; return;
  }
}

void Status::turn_left() {
  turn_right();
  turn_right();
  turn_right();
}






struct Grid {
  std::vector<std::string> lines;

  Grid(std::vector<std::string> lines) : lines(lines) {
    normalize();
  }
  
  void normalize();

  Status get_initial_status() const;
  bj::Position normalize_position(bj::Position pos) const;

};

void Grid::normalize() {
  size_t max_l = 0;
  for (const std::string & line : lines) {
    max_l = std::max(max_l, line.size());
  }

  for (std::string & line : lines) {
    line.resize(max_l, ' ');
  }
}


Status Grid::get_initial_status() const {
  Status retval { bj::Position{ 0, 0 }, bj::Direction::Right };

  while (lines[0][retval.where.x] != '.') {
    retval.where.x += 1;
  }

  return retval;
}


void Status::move(const Grid & grid) {
  bj::Position original = this->where;

  where.move(towards);
  while (true) {
    where = grid.normalize_position(where);
    if (grid.lines[where.y][where.x] == '#') {
      where = original;
      return;
    }
    if (grid.lines[where.y][where.x] == '.') {
      return;
    }
    where.move(towards);
  }
}


bj::Position Grid::normalize_position(bj::Position pos) const {
  if (pos.x == -1) pos.x = lines[0].size() - 1;
  if (pos.x == lines[0].size()) pos.x = 0;
  if (pos.y == -1) pos.y = lines.size() - 1;
  if (pos.y == lines.size()) pos.y = 0;
  return pos;
}


int score_direction(bj::Direction dir) {
  switch (dir) {
    case bj::Direction::Down: return 1;
    case bj::Direction::Left: return 2;
    case bj::Direction::Top : return 3;
    case bj::Direction::Right: return 0;
  }  
}

Output day_2022_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<std::string> grid_lines = lines;
  grid_lines.pop_back();
  grid_lines.pop_back();

  Grid grid(grid_lines);

  Status current_pos = grid.get_initial_status();

  size_t i = 0;
  int acc = 0;

  std::string instructions = lines.back();
  while (i != instructions.size()) {
    if (instructions[i] == 'L') {
      current_pos.move(grid, acc);
      current_pos.turn_left();
      acc = 0;
    } else if (instructions[i] == 'R') {
      current_pos.move(grid, acc);
      current_pos.turn_right();
      acc = 0;
    } else {
      acc = acc * 10 + (instructions[i] - '0');
    }
    ++i;
  }

  current_pos.move(grid, acc);


  const long long int part_a = (current_pos.where.y+1) * 1000 + (current_pos.where.x+1) * 4 + score_direction(current_pos.towards);

  return Output(part_a, 0);
}
