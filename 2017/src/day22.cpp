#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <vector>
#include <set>
#include <map>

// https://adventofcode.com/2017/day/22

namespace {

size_t part_a(std::set<bj::Position> infested, int mid_x, int mid_y) {
  bj::Kitten virus_carrier(bj::Position{ mid_x, mid_y }, bj::Direction::Top);

  size_t added_virus = 0;
  for (size_t i = 0; i != 10000; ++i) {
    if (infested.contains(virus_carrier.position)) {
      infested.erase(virus_carrier.position);
      virus_carrier.turn_right();
      virus_carrier.advance();
    } else {
      infested.emplace(virus_carrier.position);
      ++added_virus;
      virus_carrier.turn_left();
      virus_carrier.advance();
    }
  }

  return added_virus;
}

size_t part_b(std::set<bj::Position> infested_set, int mid_x, int mid_y) {
  enum class State { Clean, Weakened, Infected, Flagged };
  std::map<bj::Position, State> infested;
  for (const auto infested_element : infested_set) {
    infested.emplace(infested_element, State::Infected);
  }

  bj::Kitten virus_carrier(bj::Position{ mid_x, mid_y }, bj::Direction::Top);

  size_t added_virus = 0;
  for (size_t i = 0; i != 10000000; ++i) {
    auto it = infested.find(virus_carrier.position);
    State state = it == infested.end() ? State::Clean : it->second;

    if (state == State::Clean) {
      infested[virus_carrier.position] = State::Weakened;
      virus_carrier.turn_left();
      virus_carrier.advance();
    } else if (state == State::Weakened) {
      infested[virus_carrier.position] = State::Infected;
      ++added_virus;
      virus_carrier.advance();
    } else if (state == State::Infected) {
      infested[virus_carrier.position] = State::Flagged;
      virus_carrier.turn_right();
      virus_carrier.advance();
    } else {
      infested.erase(virus_carrier.position);
      virus_carrier.turn_180();
      virus_carrier.advance();
    }
  }

  return added_virus;
}

}

Output day_2017_22(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::set<bj::Position> infested;

  for (size_t y = 0; y != lines.size(); ++y) {
    for (size_t x = 0; x != lines[y].size(); ++x) {
      if (lines[y][x] == '#') {
        infested.emplace(bj::Position{ static_cast<int>(x), static_cast<int>(y) });
      }
    }
  }

  const auto result_a = part_a(infested, static_cast<int>(lines[0].size() / 2), static_cast<int>(lines.size() / 2));
  const auto result_b = part_b(infested, static_cast<int>(lines[0].size() / 2), static_cast<int>(lines.size() / 2));

  return Output(result_a, result_b);
}
