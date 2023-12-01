#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <string>

// https://adventofcode.com/2017/day/11


int distance_to_origin(bj::Position position) {
  const int horizontal_movement = std::abs(position.x);

  int total_movement = horizontal_movement;

  const int vertical_movement = std::abs(position.y);
  if (horizontal_movement < vertical_movement) {
    total_movement += (vertical_movement - horizontal_movement) / 2;
  }

  return total_movement;
}

Output day_2017_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string line = lines[0];

  std::vector<std::string> directions = bj::string_split(line, ",");

  bj::Position position { 0, 0 };

  int further_distance = 0;

  for (const std::string direction : directions) {
    if (direction == "sw") {
      position.y += 1;
      position.x -= 1;
    } else if (direction == "se") {
      position.y += 1;
      position.x += 1;
    } else if (direction == "nw") {
      position.y -= 1;
      position.x -= 1;
    } else if (direction == "ne") {
      position.y -= 1;
      position.x += 1;
    } else if (direction == "s") {
      position.y += 2;
    } else if (direction == "n") {
      position.y -= 2;
    } else {
      std::cerr << direction << "\n";
      throw std::runtime_error("unexepected direction"); // i can't type
    }
    
    further_distance = std::max(further_distance, distance_to_origin(position));
  }

  const int current_distance = distance_to_origin(position);

  return Output(current_distance, further_distance);
}
