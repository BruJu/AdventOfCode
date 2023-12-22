#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include "../../util/instruction_reader.hpp"
#include <queue>
#include <set>
#include <memory>
#include "../../util/position.hpp"

using i64 = std::int64_t;




// https://adventofcode.com/2023/day/21


bj::Position find_start(const std::vector<std::string> & lines) {
  for (int y = 0; y != lines.size(); ++y) {
    for (int x = 0; x != lines[0].size(); ++x) {
      if (lines[y][x] == 'S') {
        return bj::Position{ x, y };
      }
    }
  }
  throw std::runtime_error("No start");
}

i64 do_part_a(std::vector<std::string> lines, int initial_step) {
  struct State {
    bj::Position pos;
    int steps_left;
  };

  bj::Position start = find_start(lines);

  lines[start.y][start.x] = '.';

  std::set<bj::Position> visited_even;
  std::set<bj::Position> visited_odd;

  visited_even.emplace(start);

  std::queue<State> todo;

  todo.emplace(State{ start, initial_step });

  while (!todo.empty()) {
    const auto [pos, steps] = todo.front();
    todo.pop();


    pos.for_each_neighbour([&](bj::Position neighbour) {
      if (neighbour.x < 0 || neighbour.y < 0) return;
      if (neighbour.x >= lines[0].size() || neighbour.y >= lines.size()) return;

      if (lines[neighbour.y][neighbour.x] != '.') return;

      int s = steps - 1;
      std::set<bj::Position> & the_right_set = (s % 2 == 0) ? visited_even : visited_odd;


      if (the_right_set.contains(neighbour)) return;

      the_right_set.emplace(neighbour);
      if (s == 0) return;

      todo.emplace(State{ neighbour, s });
    });
  }

  return (initial_step % 2 == 0) ? visited_even.size() : visited_odd.size();
}




struct InfiniteGrid {
  bj::Position start;
  std::vector<std::string> lines;
  int width;
  int height;

  struct DoublePosition {
    bj::Position grid;
    bj::Position pos;

    bool operator<(const DoublePosition & other) const {
      return std::pair(grid, pos) < std::pair(other.grid, other.pos);
    }
  };


  InfiniteGrid(std::vector<std::string> lines) : lines(lines) {
    start = find_start(lines);
    width = lines[0].size();
    height = lines.size();
    this->lines[start.y][start.x] = '.';
  }

  void normalize(DoublePosition & pos) {
    if (pos.pos.x < 0) {
      pos.pos.x += width;
      pos.grid.x -= 1;
    }
    if (pos.pos.y < 0) {
      pos.pos.y += height;
      pos.grid.y -= 1;
    }
    if (pos.pos.x >= width) {
      pos.pos.x -= width;
      pos.grid.x += 1;
    }
    if (pos.pos.y >= height) {
      pos.pos.y -= height;
      pos.grid.y += 1;
    }
  }


  i64 solve(int initial_step) {
    std::vector<bj::Position> starts;

    if (initial_step % 2 == 1) {
      start.for_each_neighbour([&](bj::Position p) {
        if (lines[p.y][p.x] == '.') {
          starts.emplace_back(p);
        }
      });

      --initial_step;
    } else {
      
      starts.emplace_back(start);
    }

    // step is now odd
    // Prepare queue

    struct State {
      DoublePosition pos;
      int left_steps;
    };

    std::map<bj::Position, std::set<bj::Position>> visited;
    std::queue<State> todo;

    for (const auto & start : starts) {
      visited[bj::Position{0, 0}].emplace(start);
      if (initial_step != 0)
        todo.emplace(State { DoublePosition{ bj::Position{0, 0 }, start }, initial_step });
    }

    while (!todo.empty()) {
      const auto [dpos, steps] = todo.front();
      todo.pop();

      dpos.pos.for_each_neighbour([&](bj::Position oddpos) {
        DoublePosition odddpos { dpos.grid, oddpos };
        normalize(odddpos);
        if (lines[odddpos.pos.y][odddpos.pos.x] != '.') return;


        odddpos.pos.for_each_neighbour([&](bj::Position dest) {
          DoublePosition final_destination { odddpos.grid, dest };
          normalize(final_destination);
          if (lines[final_destination.pos.y][final_destination.pos.x] != '.') return;

          if (visited[final_destination.grid].contains(final_destination.pos)) return;

          visited[final_destination.grid].emplace(final_destination.pos);

          int new_steps = steps - 2;
          if (new_steps == 0) return;
          todo.emplace(State{ final_destination, new_steps });
        });
      });
    }

    i64 sum = 0;
    for (const auto & [a, b] : visited) {
      sum += b.size();
    }
    return sum;
  }
};


i64 do_part_b(std::vector<std::string> lines, int initial_step) {
  InfiniteGrid infinity { lines };
  return infinity.solve(initial_step);
}


Output day_2023_21(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  
  i64 part_a = do_part_a(lines, dei.part_a_extra_param);
  i64 part_b = 0;

  if (!dei.can_skip_part_B) {
    std::vector<std::pair<int, i64>> found;

    for (int i : { 0, 1, 2 }) {
      int steps = 65 + 131 * 2 * i;
      found.emplace_back(steps, do_part_b(lines, steps));
    }

    for (const auto & [x, y] : found) {
      std::cout << x << " " << y << "\n";
    }

    std::cout << "Now do some quadratric interpolation to find x = 26501365\n";
  }

  return Output(part_a, part_b);
}
