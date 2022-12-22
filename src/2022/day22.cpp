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

  int get_score() const;
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

struct FoldInstruction {
  bj::Position from;
  char dir_to_border;
  char dir_traversal;

  FoldInstruction(int x, int y, char border, char traversal)
    : from{x, y}, dir_to_border(border), dir_traversal(traversal) {}
};


struct Folds {
  using MicroPos = std::pair<bj::Position, bj::Direction>;
  std::map<MicroPos, MicroPos> out_bound;

  bool empty() const { return out_bound.empty(); }
  void add_fold(FoldInstruction f1, FoldInstruction f2);

  void add_fold(bj::Position from, bj::Position to, bj::Direction dir, bj::Position from2, bj::Position to2, bj::Direction dir2);

  Status normalize(bj::Position to, bj::Direction direction) const;
};


int compute_d(int from, int to) {
  if (from == to) return 0;
  if (from < to) return 1;
  return -1;
}

bj::Direction to_dir(char c) {
switch(c) {
  case '>': return bj::Direction::Right;
  case '<': return bj::Direction::Left;
  case 'v': return bj::Direction::Down;
  case '^': return bj::Direction::Top;
  default: throw std::runtime_error("bad dir");
}
}



void Folds::add_fold(FoldInstruction fold1, FoldInstruction fold2) {
  bj::Position from1 = fold1.from;
  bj::Position to1 = fold1.from;
  bj::Direction dir = to_dir(fold1.dir_traversal);

  bj::Position from2 = fold2.from;
  bj::Position to2 = fold2.from;
  bj::Direction dir2 = to_dir(fold2.dir_traversal);

  for (int i = 0; i != 50; ++i) {
    to1.move(to_dir(fold1.dir_to_border));
    to2.move(to_dir(fold2.dir_to_border));
  }

  add_fold(from1, to1, dir, from2, to2, dir2);
}

bj::Direction opposite_dir(bj::Direction dir) {
  switch (dir) {
    case bj::Direction::Left: return bj::Direction::Right;
    case bj::Direction::Right: return bj::Direction::Left;
    case bj::Direction::Top: return bj::Direction::Down;
    case bj::Direction::Down: return bj::Direction::Top;
  }
  throw std::runtime_error("bad dir");

}

std::ostream & operator<<(std::ostream & stream, bj::Position p) {
  return stream << "(" << p.x <<","<<p.y <<")";
}

std::ostream & operator<<(std::ostream & stream, bj::Direction p) {

  if (p == bj::Direction::Down) return stream << "v";
    if (p == bj::Direction::Left) return stream << "<";
      if (p == bj::Direction::Top) return stream << "^";
        if (p == bj::Direction::Right) return stream << ">";
        return stream << "?";
}

void Folds::add_fold(bj::Position from, bj::Position to, bj::Direction dir, bj::Position from2, bj::Position to2, bj::Direction dir2) {
  const int dx1 = compute_d(from.x, to.x);
  const int dy1 = compute_d(from.y, to.y);
  const int dx2 = compute_d(from2.x, to2.x);
  const int dy2 = compute_d(from2.y, to2.y);

  while (from != to) {
    out_bound.emplace(MicroPos{ from, dir }, MicroPos{ from2, dir2 });
    out_bound.emplace(MicroPos{ from2, opposite_dir(dir2) }, MicroPos{ from, opposite_dir(dir) });

    if (out_bound.count(MicroPos{ from2, dir2 })) {
      throw std::runtime_error(":(");
    }
    if (out_bound.count(MicroPos{ from, opposite_dir(dir) })) {
      throw std::runtime_error(":(");
    }
    
    from.x += dx1;
    from.y += dy1;
    from2.x += dx2;
    from2.y += dy2;
  }
}

Status Folds::normalize(bj::Position to, bj::Direction direction) const {
  auto key = MicroPos{ to, direction };
  auto it = out_bound.find(key);
  if (it == out_bound.end()) {
    to.move(direction);
    return Status{ to, direction };
  }
  return Status{ it->second.first,it->second.second };
}

struct Grid {
  std::vector<std::string> lines;
  Folds folds;

  Grid(std::vector<std::string> lines) : lines(lines) {
    normalize();
  }
  
  void normalize();

  Status get_initial_status() const;
  bj::Position normalize_position(bj::Position pos) const;
  int run_instructions(std::string instructions) const;
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
  Status old = *this;

  while (true) {
    if (grid.folds.empty()) {
      where.move(towards);
      where = grid.normalize_position(where);
    } else {
      *this = grid.folds.normalize(where, towards);
    }

    if (grid.lines.at(where.y).at(where.x) == '#') {
      *this = old;
      return;
    }
    if (grid.lines[where.y][where.x] == '.') {
      return;
    }
    if (!grid.folds.empty()) {
      std::cerr << where.x << " " << where.y << "\n";
      throw std::runtime_error("bad position");
    }
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
  throw std::runtime_error("score direction bad dir");
}

int Status::get_score() const {
  return (where.y+1) * 1000 + (where.x+1) * 4 + score_direction(towards);
}

int Grid::run_instructions(std::string instructions) const {
  Status current_pos = get_initial_status();

  size_t i = 0;
  int acc = 0;

  while (i != instructions.size()) {
    if (instructions[i] == 'L') {
      current_pos.move(*this, acc);
      current_pos.turn_left();
      acc = 0;
    } else if (instructions[i] == 'R') {
      current_pos.move(*this, acc);
      current_pos.turn_right();
      acc = 0;
    } else {
      acc = acc * 10 + (instructions[i] - '0');
    }
    ++i;
  }

  current_pos.move(*this, acc);

  return current_pos.get_score();
}



Output day_2022_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<std::string> grid_lines = lines;
  grid_lines.pop_back();
  grid_lines.pop_back();

  Grid grid(grid_lines);

  std::string instructions = lines.back();

  const auto part_a = grid.run_instructions(instructions);


  int part_b = 0;

  if (lines.size() > 100) {
    grid.folds.add_fold(FoldInstruction( 50,   0, '>', '^'), FoldInstruction(  0, 150, 'v', '>')); // Orange
    grid.folds.add_fold(FoldInstruction(100,   0, '>', '^'), FoldInstruction(  0, 199, '>', '^')); // Vert
    grid.folds.add_fold(FoldInstruction(149,  49, '^', '>'), FoldInstruction( 99, 100, 'v', '<')); // Jaune
    grid.folds.add_fold(FoldInstruction( 99,  50, 'v', '>'), FoldInstruction(100,  49, '>', '^')); // Violet
    grid.folds.add_fold(FoldInstruction( 50,  49, '^', '<'), FoldInstruction(  0, 100, 'v', '>')); // Noir
    grid.folds.add_fold(FoldInstruction( 50, 149, '>', 'v'), FoldInstruction( 49, 150, 'v', '<')); // Rouge
    grid.folds.add_fold(FoldInstruction( 50,  99, '^', '<'), FoldInstruction( 49, 100, '<', 'v')); // Bleu

    part_b = grid.run_instructions(instructions);
  }

  return Output(part_a, part_b);
}

// >= 45316
