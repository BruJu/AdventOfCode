#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <set>
#include <list>
#include <array>
#include <optional>
#include <bitset>
#include <array>
#include <span>

// https://adventofcode.com/2022/day/17


struct Grid {
  std::set<bj::Position> m_stabilized_positions;
  std::vector<bj::Position> current_falling_position;
  int number_of_rocks = 0;


  bool on_descend();
  void jet_stream(int dx);

  

  int get_highest_y();

  // [[nodiscard]] int number_of_rocks() const { return number_of_rocks; }

  bool can_move(int dx, int dy) const;
  void move(int dx, int dy);


  void draw(int max_y);

  int get_height_score(int y) {
    int score = 0;

    for (int x = 0; x < 7; ++x) {
      if (m_stabilized_positions.contains(bj::Position{ x, y })) {
        score += 1 << (x);
      }
    }

    return score;
  }

  static constexpr int tetris_score = 0x7F;

};

void Grid::draw(int max_y) {
  for (int y = max_y; y >= -1; --y) {
    if (y != -1) std::cout << '|'; else std::cout << '+';
    for (int x = 0; x < 7; ++x) {
      bj::Position pos { x, y };

      if (y == -1) std::cout << '-';
      else if (m_stabilized_positions.contains(pos)) {
        std::cout << '#';
      } else if (auto it = std::find(current_falling_position.begin(), current_falling_position.end(), pos); it != current_falling_position.end()) {
        std::cout << 'v';
      } else {
        std::cout << ' ';
      }
    }

    if (y != -1) std::cout << '|'; else std::cout << '+';
    std::cout << "\n";
  }
}

void Grid::jet_stream(int dx) {
  if (can_move(dx, 0)) {
    move(dx, 0);
  }
}

void Grid::move(int dx, int dy) {
  for (bj::Position & pos : current_falling_position) {
    pos.x += dx;
    pos.y += dy;
  }
}

int Grid::get_highest_y() {
  int highest = 0;

  for (const auto & position : m_stabilized_positions) {
    if (position.y + 1> highest) {
      highest = position.y + 1;
    }
  }

  return highest;
}

bool Grid::on_descend() {
  if (can_move(0, -1)) {
    for (bj::Position & pos : current_falling_position) {
      pos.y -= 1;
    }

    return true;
  } else {
    for (const bj::Position & pos : current_falling_position) {
      m_stabilized_positions.emplace(pos);
    }

    current_falling_position.clear();
    ++number_of_rocks;

    return false;
  }
}


bool Grid::can_move(int dx, int dy) const {
  for (const bj::Position & pos : current_falling_position) {
    bj::Position delta = pos;
    delta.x += dx;
    delta.y += dy;

    if (delta.x < 0) return false;
    if (delta.x >= 7) return false;
    if (delta.y < 0) return false;
    if (m_stabilized_positions.contains(delta)) return false;
  }

  return true;
}

const char * falling_rock_1 = "####";
const char * falling_rock_2 = ".#. ### .#.";
const char * falling_rock_3 = "..# ..# ###";
const char * falling_rock_4 = "# # # #";
const char * falling_rock_5 = "## ##";

std::array<const char *, 5> falling_types = {
  falling_rock_1, falling_rock_2, falling_rock_3, falling_rock_4, falling_rock_5
};

struct JetPattern {
  std::string pattern;
  size_t i = 0;

  [[nodiscard]] char next() {
    const char p = pattern[i];
    i = (i + 1) % pattern.size();
    return p;
  }
};

struct RockAdder {
  size_t i = 0;

  void add_rock(Grid & grid) {
    const char * rock_type = falling_types[i];
    i = (i + 1) % falling_types.size();

    int x = 2;
    int y = grid.get_highest_y() + 3;

    while (*rock_type) {
      if (*rock_type == ' ') {
        x = 2;
        
        for (bj::Position & pos : grid.current_falling_position) {
          ++pos.y;
        }
      } else if (*rock_type == '#') {
        grid.current_falling_position.emplace_back(bj::Position{ x, y });
        ++x;
      } else {
        ++x;
      }
      ++rock_type;
    }
  }
};



Output day_2022_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string & line = lines[0];

  JetPattern pattern(line);
  RockAdder rock_faller;

  Grid grid;

  int x=  0;
  while (grid.number_of_rocks != 2022) {
    
    //std::cout << x <<  ' ' << grid.number_of_rocks << "\n";
    ++x;
    if (grid.current_falling_position.size() == 0) {
      rock_faller.add_rock(grid);
    }
    //grid.draw(grid.get_highest_y() + 6);

    char stream = pattern.next();
    if (stream == '<') {
      grid.jet_stream(-1);
    } else if (stream == '>') {
      grid.jet_stream(1);
    } else {
      throw std::runtime_error(":(");
    }

    grid.on_descend();
  }

  
  return Output(grid.get_highest_y(), 0);
}
