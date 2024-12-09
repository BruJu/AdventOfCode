#include "../../common/advent_of_code.hpp"
#include <map>
#include <set>
#include "../../util/position.hpp"

// https://adventofcode.com/2024/day/8

namespace {

bool is_antenna(char c) {
  return (c >= '0' && c <= '9')
    || (c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z');
}

auto find_antennas(const std::vector<std::string> & lines) {
  std::map<char, std::set<bj::Position>> retval;

  for (size_t i = 0; i != lines.size(); ++i) {
    for (size_t j = 0; j != lines[i].size(); ++j) {
      char c = lines[i][j];
      if (is_antenna(c)) {
        retval[c].emplace(bj::Position{ static_cast<int>(i), static_cast<int>(j) });
      }
    }
  }

  return retval;
}

template<bool with_loop>
void add_interferences(
  std::set<bj::Position> & out_interferences,
  std::set<bj::Position> antennas,
  bj::Position bottom_right_bound
) {
  for (const bj::Position & left : antennas) {
    for (const bj::Position & right : antennas) {
      if (left == right) continue;

      bj::Position interference = left;

      if (with_loop) {
        out_interferences.emplace(interference);
      }

      do {
        interference.x -= (right.x - left.x);
        interference.y -= (right.y - left.y);

        if (interference.x < 0) break;
        if (interference.y < 0) break;
        if (interference.x >= bottom_right_bound.x) break;
        if (interference.y >= bottom_right_bound.y) break;

        out_interferences.emplace(interference);
      } while (with_loop);
    }
  }
}

}

Output day_2024_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto all_antennas = find_antennas(lines);
  size_t right_bound = lines[0].size();
  size_t bottom_bound = lines.size();
  bj::Position bottom_right_bound{
    static_cast<int>(right_bound),
    static_cast<int>(bottom_bound)
  };
  
  std::set<bj::Position> interferences_first;
  std::set<bj::Position> interferences_full_line;
  for (const auto & [type, antennas] : all_antennas) {
    add_interferences<false>(interferences_first, antennas, bottom_right_bound);
    add_interferences<true>(interferences_full_line, antennas, bottom_right_bound);
  }

  return Output(
    interferences_first.size(),
    interferences_full_line.size()
    ); 
}
