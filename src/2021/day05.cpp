#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include "../util/instruction_reader.hpp"
#include <algorithm>
#include <optional>
#include <map>
#include <array>
#include <numeric>

// https://adventofcode.com/2021/day/5
// a,b -> c,d
// => We draw a line drop the point (a,b) to the point (c,d)
// On how many points at least two line cross:
// part a: if we only consider straigh lines
// part b: if we consider all lines

/** A line defined from two positions */
struct DrawLine {
  bj::Position from;
  bj::Position to;
};

/** Let's use too powerfull abstractions */
static std::vector<DrawLine> read_input(const std::vector<std::string> & lines) {
  bj::InstructionReader<DrawLine> reader;

  reader.add_handler(
    R"(([0-9]+),([0-9]+) -> ([0-9]+),([0-9]+))",
    [](const std::vector<std::string> & values) -> DrawLine {
      const bj::Position top_left = { std::stoi(values[0]), std::stoi(values[1]) };
      const bj::Position bottom_right = { std::stoi(values[2]), std::stoi(values[3]) };
      return { top_left, bottom_right };
    }
  );

  return reader(lines).value();
}

/** Calls callable for each point in the interval [from, to] */
template <typename Callable>
void for_all_points_between(bj::Position from, bj::Position to, Callable callable) {
  const int dx = from.x == to.x ? 0 : from.x < to.x ? 1 : -1;
  const int dy = from.y == to.y ? 0 : from.y < to.y ? 1 : -1;

  const bool is_straight = dx == 0 || dy == 0;

  bj::Position i = from;

  while (i != to) {
    callable(i, is_straight);
    i.x += dx;
    i.y += dy;
  }

  callable(i, is_straight);
}

/**
 * Solves the problem using an approach with std::map.
 * 
 * Very straight forward, but takes a long time: 76~84ms.
 */
[[maybe_unused]] static Output solve_with_map(const std::vector<DrawLine> & lines_to_draw) {
  // = Draw the lines
  std::map<bj::Position, int> straight_lines;
  std::map<bj::Position, int> all_lines;

  for (const auto & [from, to] : lines_to_draw) {
    for_all_points_between(from, to,
      [&](bj::Position point, bool is_straight) {
        if (is_straight) {
          ++straight_lines[point];
        }

        ++all_lines[point];
      }
    );
  }

  // = Get the answer
  static constexpr auto get_answer_map = [](const std::map<bj::Position, int> & map) -> size_t {
    return std::count_if(map.begin(), map.end(),
      [&](const std::map<bj::Position, int>::value_type & pair) {
        return pair.second >= 2;
      }
    );
  };

  const auto part_a = get_answer_map(straight_lines);
  const auto part_b = get_answer_map(all_lines);
  
  return Output(part_a, part_b);
}

/**
 * Solve the problem using a grid stored in a 2D std::vector.
 * 
 * Takes 5~14 ms.
 */
static Output solve_with_vector(const std::vector<DrawLine> & lines_to_draw) {
  // = Find bottom left corner
  int y_max = 0;
  int x_max = 0;
  for (const auto & [from, to] : lines_to_draw) {
    if (to.y > y_max) y_max = to.y;
    if (to.x > x_max) x_max = to.x;
    if (from.y > y_max) y_max = from.y;
    if (from.x > x_max) x_max = from.x;
  }

  // = Build the grids
  std::vector<std::vector<int>> straight_lines;
  std::vector<std::vector<int>> all_lines;

  for (int x = 0; x <= x_max; ++x) {
    straight_lines.emplace_back(size_t(y_max + 1), 0);
    all_lines.emplace_back(size_t(y_max + 1), 0);
  }

  // = Draw
  for (const auto & [from, to] : lines_to_draw) {
    for_all_points_between(from, to,
      [&](bj::Position point, bool is_straight) {
        if (is_straight) {
          ++straight_lines[point.x][point.y];
        }

        ++all_lines[point.x][point.y];
      }
    );
  }

  // = Get the answer
  static constexpr auto get_answer_vector = [](const std::vector<std::vector<int>> & values) {
    static constexpr auto more_than_2 = [](int value) { return value >= 2; };
    static constexpr auto count_more_than_2 = [](const std::vector<int> & values) {
      return std::count_if(values.begin(), values.end(), more_than_2);
    };

    size_t answer = 0;
    for (const auto & column : values) {
      answer += count_more_than_2(column);
    }
    return answer;
  };

  const auto part_a = get_answer_vector(straight_lines);
  const auto part_b = get_answer_vector(all_lines);
  
  return Output(part_a, part_b);
}

Output day_2021_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto lines_to_draw = read_input(lines);

  // return solve_with_map(lines_to_draw); // same answer but slower
  return solve_with_vector(lines_to_draw);
}
