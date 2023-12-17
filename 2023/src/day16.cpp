#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <string_view>
#include <charconv>
#include "../../util/position.hpp"

using i64 = std::int64_t;
using Grid = std::vector<std::string>;

#pragma GCC diagnostic ignored "-Wsign-compare"

// https://adventofcode.com/2023/day/16


static bad_static_vector<bj::Direction, 2> get_next(char c, const bj::Direction dir) {
  bad_static_vector<bj::Direction, 2> retval;
  if (c == '.') {
    retval.add(dir);
  } else if (c == '|') {
    if (dir == bj::Direction::Top || dir == bj::Direction::Down) {
      retval.add(dir);
    } else {
      retval.add(bj::Direction::Top);
      retval.add(bj::Direction::Down);
    }
  } else if (c == '-') {
    if (dir == bj::Direction::Left || dir == bj::Direction::Right) {
      retval.add(dir);
    } else {
      retval.add(bj::Direction::Left);
      retval.add(bj::Direction::Right);
    }
  } else if (c == '/') {
    if (dir == bj::Direction::Left) {
      retval.add(bj::Direction::Down);
    } else if (dir == bj::Direction::Right) {
      retval.add(bj::Direction::Top);
    } else if (dir == bj::Direction::Top) {
      retval.add(bj::Direction::Right);
    } else if (dir == bj::Direction::Down) {
      retval.add(bj::Direction::Left);
    }
  } else if (c == '\\') {
    if (dir == bj::Direction::Left) {
      retval.add(bj::Direction::Top);
    } else if (dir == bj::Direction::Right) {
      retval.add(bj::Direction::Down);
    } else if (dir == bj::Direction::Top) {
      retval.add(bj::Direction::Left);
    } else if (dir == bj::Direction::Down) {
      retval.add(bj::Direction::Right);
    }
  } else {
    throw std::runtime_error("????");
  }
  return retval;
}


struct Beam : bj::Kitten {
  [[nodiscard]] bool operator<(const Beam & other) const {
    return std::pair(direction, position) < std::pair(other.direction, other.position);
  }
};

void draw_solution(const std::vector<std::string> & lines, std::set<bj::Position> lighten) {
  for (int y = 0; y != lines.size(); ++y) {
    for (int x = 0; x != lines[0].size(); ++x) {
      bj::Position pos { x, y };

      if (lighten.contains(pos)) {
        std::cout << '#';
      } else {
        std::cout << '.';
      }
    }
    std::cout << '\n';
  }
}

i64 solve(const std::vector<std::string> & lines, Beam first_beam) {
  std::set<bj::Position> lighten;
  std::set<Beam> visited;

  std::vector<Beam> beams;

  beams.emplace_back(first_beam);
  lighten.emplace(first_beam.position);

  while (!beams.empty()) {
    auto beam = beams.back();
    beams.erase(beams.end() - 1);

    const auto next_dirs = get_next(lines[beam.position.y][beam.position.x], beam.direction);

    for (const bj::Direction & next_dir : next_dirs.get_all()) {
      Beam copy = beam;
      copy.direction = next_dir;
      copy.advance();

      if (copy.position.x < 0) continue;
      if (copy.position.y < 0) continue;

      if (copy.position.x >= lines[0].size()) continue;
      if (copy.position.y >= lines.size()) continue;

      if (visited.contains(copy)) continue;
      
      lighten.emplace(copy.position);
      visited.emplace(copy);
      beams.emplace_back(copy);
    }
  }

  return lighten.size();
}

Output day_2023_16(const std::vector<std::string> & lines, const DayExtraInfo &) { 
  i64 part_a = solve(lines, Beam{ bj::Kitten( bj::Position{0, 0}, bj::Direction::Right ) } );

  i64 part_b = 0;

  for (int y = 0; y != lines.size(); ++y) {
    i64 a = solve(lines, Beam{ bj::Kitten( bj::Position{0, y}, bj::Direction::Right ) } );
    i64 b = solve(lines, Beam{ bj::Kitten( bj::Position{int(lines[0].size() - 1), y}, bj::Direction::Left ) } );

    part_b = std::max({ part_b, a, b });
  }

  for (int x = 0; x != lines[0].size(); ++x) {
    i64 a = solve(lines, Beam{ bj::Kitten( bj::Position{x, 0}, bj::Direction::Down ) } );
    i64 b = solve(lines, Beam{ bj::Kitten( bj::Position{x, int(lines.size() - 1)}, bj::Direction::Top ) } );
    
    part_b = std::max({ part_b, a, b });
  }

  return Output(part_a, part_b);
}
