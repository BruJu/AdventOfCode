#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>

// https://adventofcode.com/2017/day/03

namespace {

bool is_in(bj::Position pos, bj::Rectangle rectangle) {
  return rectangle.left <= pos.x
    && rectangle.right >= pos.x
    && rectangle.top <= pos.y
    && rectangle.bottom >= pos.y;
}

void turn_left(bj::Direction & direction) {
  switch (direction) {
    case bj::Direction::Left: direction = bj::Direction::Down; return;
    case bj::Direction::Down: direction = bj::Direction::Right; return;
    case bj::Direction::Right: direction = bj::Direction::Top; return;
    case bj::Direction::Top: direction = bj::Direction::Left; return;
  }
}

size_t abs(bj::Position position) {
  return std::abs(position.x) + std::abs(position.y);
}

bj::Position part_a(int input) {
  bj::Position position{ 0, 0 };
  int current_position = 1;

  bj::Rectangle rectangle { 0, 0, 0, 0 };
  bj::Direction current_direction = bj::Direction::Right;

  while (current_position != input) {
    ++current_position;
    position.move(current_direction);

    if (is_in(position, rectangle)) {
      // ok
    } else {
      turn_left(current_direction);
      rectangle.left = std::min(rectangle.left, position.x);
      rectangle.right = std::max(rectangle.right, position.x);
      rectangle.top = std::min(rectangle.top, position.y);
      rectangle.bottom = std::max(rectangle.bottom, position.y);
    }
  }
  
  return position;
}

int part_b(int input) {
  std::map<bj::Position, int> layout;
  layout[bj::Position{ 0, 0 }] = 1;

  bj::Position position{ 0, 0 };
  int current_position = 1;

  bj::Rectangle rectangle { 0, 0, 0, 0 };
  bj::Direction current_direction = bj::Direction::Right;

  while (true) {
    ++current_position;
    position.move(current_direction);

    int sum = 0;
    for (const auto & neighbour : position.get_8_neighbours()) {
      auto it = layout.find(neighbour);
      if (it != layout.end()) {
        sum += it->second;
      }
    }
    
    if (sum >= input) return sum;

    layout.emplace(position, sum);

    if (is_in(position, rectangle)) {
      // ok
    } else {
      turn_left(current_direction);
      rectangle.left = std::min(rectangle.left, position.x);
      rectangle.right = std::max(rectangle.right, position.x);
      rectangle.top = std::min(rectangle.top, position.y);
      rectangle.bottom = std::max(rectangle.bottom, position.y);
    }
  }
}

}

Output day_2017_03(const std::vector<std::string> &, const DayExtraInfo & dei) {
  const int input = dei.part_a_extra_param;
  const auto part_a_ = part_a(input);
  
  return Output(abs(part_a_), part_b(input));
}

