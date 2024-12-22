#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
// #include <map>
// #include <cstring>
// #include <utility>
#include <climits>
#include <queue>
// #include <span>

// https://adventofcode.com/2024/day/17

namespace {


std::vector<std::vector<std::optional<int>>> to_step_map(const std::vector<std::string> & grid) {
  std::vector<std::vector<std::optional<int>>> output;
  output.reserve(grid.size());

  for (const std::string & line : grid) {
    output.emplace_back();
    output.back().reserve(line.size());
    for (const char c : line) {
      if (c == '#') {
        output.back().emplace_back(INT_MAX);
      } else{
        output.back().emplace_back(std::nullopt);
      }
    }
  }

  return output;
}



int steps_to_reach_the_end(const std::vector<std::string> & grid) {
  auto step_map = to_step_map(grid);

  step_map[0][0] = 0;

  std::queue<bj::Position> todo;
  todo.emplace(bj::Position{ 0, 0 });

  while (!todo.empty()) {
    bj::Position pos = todo.front();
    todo.pop();
    const int neighbour_cost = *step_map[pos.y][pos.x] + 1;

    pos.for_each_neighbour([&](bj::Position neighbour) {
      std::optional<int> & neighbour_known_cost = step_map[neighbour.y][neighbour.x];

      if (neighbour.is_out_of_bound(grid)) return;
      if (grid[neighbour.y][neighbour.x] == '#') return;
      if (neighbour_known_cost && *neighbour_known_cost <= neighbour_cost) return;

      neighbour_known_cost = neighbour_cost;
      todo.emplace(neighbour);
    });
  }

  return step_map.back().back().value_or(-1);
}

std::vector<std::string> make_mock_grid(int size) {
  std::vector<std::string> out;

  for (int i = 0; i <= size; ++i) {
    out.emplace_back();
    for (int j = 0; j <= size; ++j) {
      out.back() += '.';
    }
  }

  return out;
}


void reset_mock_grid(std::vector<std::string> & grid) {
  for (char & c : grid[0]) {
    c = '.';
  }

  for (std::string & line : grid) {
    line = grid[0];
  }
}

void set_mock_grid_for(std::span<const bj::Position> corruptions, std::vector<std::string> & grid) {
  reset_mock_grid(grid);

  for (bj::Position pos : corruptions) {
    grid[pos.y][pos.x] = '#';
  }
}

int get_result_for(std::span<const bj::Position> corruptions, std::vector<std::string> & grid) {
  set_mock_grid_for(corruptions, grid);
  return steps_to_reach_the_end(grid);
}

}

Output day_2024_18(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  int size = dei.part_a_extra_param == 0 ? 70 : dei.part_a_extra_param;

  std::vector<std::string> mock_grid = make_mock_grid(size);

  std::vector<bj::Position> corruptions;
  for (const std::string & line : lines) {
    std::vector<int> values = bj::extract_ints_from_line(line);
    corruptions.emplace_back(bj::Position{ values[0], values[1] });
  }

  const int part_a = get_result_for(std::span(corruptions.data(), size == 6 ? 12 : 1024), mock_grid);

  // Dichotomic search but the linear search is not that long actually
  size_t left = 0;
  size_t right = corruptions.size();
  while (true) {
    if (left + 1 >= right) break;

    size_t middle = (right - left) / 2;
    int r = get_result_for(std::span(corruptions.data(), left + middle), mock_grid);
    if (r == -1) {
      right = left + middle;
    } else {
      left = left + middle;
    }
  }

  return Output(part_a, std::to_string(corruptions[left].x) + ',' + std::to_string(corruptions[left].y));
}
