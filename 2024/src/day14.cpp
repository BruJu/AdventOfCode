#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <algorithm>
#include <map>
#include <regex>

// https://adventofcode.com/2024/day/14

namespace {

struct Robot {
  bj::Position position;
  bj::Position velocity;

  void move(bj::Position bottom_right) {
    position += velocity;
    get_back_in_bounds(bottom_right);
  }

  static void move(std::span<Robot> robots, bj::Position bottom_right) {
    for (Robot & robot : robots) {
      robot.move(bottom_right);
    }
  }

private:
  void get_back_in_bounds(bj::Position bottom_right) {
    while (position.x < 0) position.x += bottom_right.x;
    while (position.y < 0) position.y += bottom_right.y;
    if (position.x >= bottom_right.x) position.x = position.x % bottom_right.x;
    if (position.y >= bottom_right.y) position.y = position.y % bottom_right.y;
  }
};

struct LineReader {
  std::regex pattern { R"(^p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+)$)" } ;
  std::smatch matches;

  [[nodiscard]] Robot operator()(const std::string & line) {
    if (!std::regex_search(line, matches, pattern))
      throw std::runtime_error("2024-04 unknown line " + line);

    const int a = std::stoi(matches[1].str());
    const int b = std::stoi(matches[2].str());
    const int c = std::stoi(matches[3].str());
    const int d = std::stoi(matches[4].str());
    return Robot {
      bj::Position { a, b },
      bj::Position { c, d }
    };
  }

  [[nodiscard]] std::vector<Robot> operator()(const std::vector<std::string> & lines) {
    std::vector<Robot> r;
    for (const auto & line : lines) {
      r.emplace_back(operator()(line));
    }
    return r;
  }
};

void draw(const std::vector<std::string> & grid) {
  for (const std::string & line : grid) {
    printf("%s\n", line.c_str());
  }
  printf("\n");
}

long long int after_100_seconds(std::vector<Robot> robots, bj::Position bottom_right) {
  for (int second = 0; second != 100; ++second) {
    Robot::move(robots, bottom_right);
  }

  std::map<bj::Position, long long int> quadrant;
  bj::Position center { bottom_right.x / 2, bottom_right.y / 2 };
  for (const Robot & robot : robots) {
    const int x = robot.position.x < center.x ? -1 : robot.position.x > center.x ? 1 : 0;
    const int y = robot.position.y < center.y ? -1 : robot.position.y > center.y ? 1 : 0;

    quadrant[bj::Position{ x, y }] += 1;
  }

  return 1
    * quadrant[bj::Position{ -1, -1 }]
    * quadrant[bj::Position{ -1, 1 }]
    * quadrant[bj::Position{  1, -1 }]
    * quadrant[bj::Position{  1, 1 }];
}

long long int find_the_tree(std::vector<Robot> robots, bj::Position bottom_right) {
  // Empty grid
  std::vector<std::string> empty_grid;
  for (int i = 0; i != bottom_right.y; ++i) {
    empty_grid.emplace_back();
    for (int j = 0; j != bottom_right.x; ++j) {
      empty_grid.back() += '.';
    }
  }

  // Find the tree
  int second = 0;
  while (second < 50000) {
    second += 1;
    
    auto grid = empty_grid;

    Robot::move(robots, bottom_right);

    for (const Robot & robot : robots) {
      grid[robot.position.y][robot.position.x] = 'x';
    }

    const auto has_tree = std::ranges::any_of(
      grid, [](const std::string & line) { return line.contains("xxxxxxxxxxxx"); }
      );

    if (has_tree) {
      draw(grid);
      return second;
    }
  }

  return -1;
}


}

Output day_2024_14(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  int wide = dei.part_a_extra_param;
  int tall = wide == 11 ? 7 : 103;

  bj::Position bottom_right { wide, tall };

  std::vector<Robot> robots = LineReader()(lines);

  long long int part_a = after_100_seconds(robots, bottom_right);

  long long int part_b = 0;
  if (wide > 20)
    part_b = find_the_tree(robots, bottom_right);

  return Output(part_a, part_b);
}
