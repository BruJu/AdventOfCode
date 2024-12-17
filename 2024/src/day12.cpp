#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <set>
#include <stack>

// https://adventofcode.com/2024/day/12

namespace {

long long int get_number_of_sides(std::set<bj::Position> visited) {
  long long int sides = 0;

  // TODO instead of copy paste
  // static constexpr auto check_side = [](
  //   const std::set<bj::Position> & visited,
  //   bj::Position me,
  //   int delta_x_side, int delta_y_side,
  //   int delta_x_move_up, int delta_y_move_up
  // ) {
  //   
  // 
  // };

  for (const bj::Position me : visited) {

    {
      // left side
      bj::Position left = me; left.x -= 1;
      bj::Position up   = me; up.y -= 1;
      bj::Position left_up = me; left_up.x -=1; left_up.y -=1;
      if (!visited.contains(left)) {
        if (!visited.contains(up)) {
          ++sides;
        } else {
          if (visited.contains(left_up)) {
            ++sides;
          }
        }
      }
    }


    {
      // right side
      bj::Position left = me; left.x += 1;
      bj::Position up   = me; up.y -= 1;
      bj::Position left_up = me; left_up.x +=1; left_up.y -=1;
      if (!visited.contains(left)) {
        if (!visited.contains(up)) {
          ++sides;
        } else {
          if (visited.contains(left_up)) {
            ++sides;
          }
        }
      }
    }

    
    {
      // top side
      bj::Position left = me; left.y -= 1;
      bj::Position up   = me; up.x -= 1;
      bj::Position left_up = me; left_up.y -=1; left_up.x -=1;
      if (!visited.contains(left)) {
        if (!visited.contains(up)) {
          ++sides;
        } else {
          if (visited.contains(left_up)) {
            ++sides;
          }
        }
      }
    }


    {
      // bottom side
      bj::Position left = me; left.y += 1;
      bj::Position up   = me; up.x += 1;
      bj::Position left_up = me; left_up.y +=1; left_up.x +=1;
      if (!visited.contains(left)) {
        if (!visited.contains(up)) {
          ++sides;
        } else {
          if (visited.contains(left_up)) {
            ++sides;
          }
        }
      }
    }

  }


  return sides;
}

std::pair<
  long long int,
  long long int
> fence(
  std::vector<std::string> & map,
  bj::Position position
) {
  const char plant = map[position.y][position.x];
  if (plant == '.') return {0,0};

  long long int perimeter = 0;
  long long int area = 0;

  std::set<bj::Position> visited;
  std::vector<bj::Position> to_remove;
  std::stack<bj::Position> todo;
  todo.emplace(position);
  visited.emplace(position);
  
  while (!todo.empty()) {
    bj::Position pos = todo.top();
    todo.pop();

    to_remove.emplace_back(pos);
    ++area;

    pos.for_each_neighbour([&](const bj::Position neighbour) {

      if (neighbour.is_out_of_bound(map)) {
        ++perimeter;
        return;
      }

      if (map[neighbour.y][neighbour.x] != plant) {
        ++perimeter;
        return;
      }

      if (visited.contains(neighbour)) return;
      visited.emplace(neighbour);
      todo.emplace(neighbour);
    });
  }

  for (bj::Position pos : to_remove) {
    map[pos.y][pos.x] = '.';
  }
  
  long long int number_of_sides = get_number_of_sides(visited);


  return { perimeter * area, number_of_sides * area };
}



}

Output day_2024_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;
  long long int part_b = 0;

  auto copy = lines;

  for (size_t y = 0; y != lines.size(); ++y) {
    for (size_t x = 0; x != lines[0].size(); ++x) {
      const auto [a, b] = fence(copy, bj::Position::from(x, y));
      part_a += a;
      part_b += b;
    }
  }

  return Output(part_a, part_b); 
}
