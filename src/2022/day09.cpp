#include "../advent_of_code.hpp"
#include <optional>
#include <algorithm>
#include <set>
#include "../util/position.hpp"
#include <ranges>

// https://adventofcode.com/2022/day/9

static bj::Direction read_direction(const char c) {
  if (c == 'R') return bj::Direction::Right;
  if (c == 'U') return bj::Direction::Top;
  if (c == 'L') return bj::Direction::Left;
  if (c == 'D') return bj::Direction::Down;
  throw std::runtime_error("Bad symbol for read_direction");
}

static void follow(const bj::Position & head, bj::Position & tail) {
  if (std::abs(tail.x - head.x) > 1 || std::abs(tail.y - head.y) > 1) {
    if (tail.x < head.x) ++tail.x;
    else if (tail.x > head.x) --tail.x;

    if (tail.y < head.y) ++tail.y;
    else if (tail.y > head.y) --tail.y;
  }
}

Output day_2022_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::set<bj::Position> visited_by_second;
  std::set<bj::Position> visited_by_last;

  std::vector<bj::Position> snake(10, bj::Position{});

  visited_by_second.emplace(snake[1]);
  visited_by_last  .emplace(snake[9]);

  for (const std::string & line : lines) {
    const bj::Direction dir = read_direction(line[0]);
    const int step = std::stoi(line.substr(2));

    for (int i = 0; i != step; ++i) {
      snake[0].move(dir);

      //for (auto & [head, tail] : snake | std::views::adjacent<2>) {
      //  follow(head, tail);
      //}

      for (size_t j = 1; j != snake.size(); ++j) {
        follow(snake[j - 1], snake[j]);
      }

      visited_by_second.emplace(snake[1]);
      visited_by_last  .emplace(snake[9]);
    }
  }
  
  return Output(visited_by_second.size(), visited_by_last.size());
}
