#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <set>
#include <utility>

// https://adventofcode.com/2024/day/6

using Grid = std::vector<std::string>;

static
char get_at(const Grid & grid, bj::Position position) {
  return grid[position.y][position.x];
}

static
bj::Position find_guard(const Grid & grid) {
  for (size_t y = 0; y != grid.size(); ++y) {
    for (size_t x = 0; x != grid[0].size(); ++x) {
      if (grid[y][x] == '^') {
        return bj::Position{ int(x), int(y) };
      }
    }
  }

  throw std::runtime_error("2024-06 no guard");
}

static
bool obstacle_here_provokes_loop(bj::Kitten guard, const bj::Position extra_obstacle, const Grid & grid) {
  std::set<std::pair<bj::Direction, bj::Position>> already_visited;

  while (true) {
    if (already_visited.contains(std::pair(guard.direction, guard.position))) {
      // cycle
      return true;
    }

    already_visited.emplace(std::pair(guard.direction, guard.position));

    guard.advance();
    if (guard.position.is_out_of_bound(grid)) {
      // No cycle
      return false;
    }

    if (guard.position == extra_obstacle || get_at(grid, guard.position) == '#') {
      // Obstacle
      guard.move_backward();
      guard.turn_right();
    }
  }
}


Output day_2024_06(const std::vector<std::string> & lines, const DayExtraInfo &) {
  
  bj::Position guard_pos = find_guard(lines);

  bj::Kitten guard(guard_pos, bj::Direction::Top);
  bj::Kitten guard_default_position(guard_pos, bj::Direction::Top);

  std::set<bj::Position> visited;
  std::set<bj::Position> obstacle_that_would_provoke_a_cycle, safe_place;
  while (true) {
    visited.emplace(guard.position);

    guard.advance();

    if (guard.position.is_out_of_bound(lines)) break;

    char c = get_at(lines, guard.position);

    if (c == '#') {
      guard.move_backward();
      guard.turn_right();
    } else if (c == '.') {
      if (!obstacle_that_would_provoke_a_cycle.contains(guard.position)
        && !safe_place.contains(guard.position)
      ) {
        if (obstacle_here_provokes_loop(guard_default_position, guard.position, lines)) {
          obstacle_that_would_provoke_a_cycle.emplace(guard.position);
        } else {
          safe_place.emplace(guard.position);
        }
      }
    }
  }

  return Output(visited.size(), obstacle_that_would_provoke_a_cycle.size());
}

// not 1792

