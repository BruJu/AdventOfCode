#include "../advent_of_code.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include "../util/position.hpp"
#include <set>
#include <span>
#include <array>

// https://adventofcode.com/2022/day/23



struct ElvesInAVolcano {
  std::set<bj::Position> elves;

  bool run_a_round(size_t round_id);

  [[nodiscard]] size_t empty_in_delimited() const;

  void draw_grid();

private:
  std::map<bj::Position, std::set<bj::Position>> make_propositions(std::array<bj::Direction, 4> directions) const;
  bool move_the_elves(const std::map<bj::Position, std::set<bj::Position>> & propositions);
};


// Gives the two directions that are perpendicual to direction
std::pair<bj::Direction, bj::Direction> travel_directions(bj::Direction direction) {
  if (direction == bj::Direction::Down || direction == bj::Direction::Top) {
    return { bj::Direction::Left, bj::Direction::Right };
  } else {
    return { bj::Direction::Down, bj::Direction::Top };
  }
}


bool ElvesInAVolcano::run_a_round(size_t round_id) {
  // I would have done a constexpr array of 7 directions if VSCode was not making compile error for std::span
  std::array<bj::Direction, 4> directions { bj::Direction::Top, bj::Direction::Down, bj::Direction::Left, bj::Direction::Right };

  for (size_t i = 0; i != round_id % 4; ++i) {
    for (size_t j = 0; j != 3; ++j) {
      std::swap(directions[j], directions[j + 1]);
    }
  }

  // ok
  const auto propal = make_propositions(directions);
  return move_the_elves(propal);
}

std::map<bj::Position, std::set<bj::Position>> ElvesInAVolcano::make_propositions(std::array<bj::Direction, 4> directions) const {
  std::map<bj::Position, std::set<bj::Position>> retval;

  for (const bj::Position & elve : elves) {
    const auto neighbours = elve.get_8_neighbours();
    
    bool is_annoyed = std::any_of(neighbours.begin(), neighbours.end(), [&](bj::Position pos) {
      return elves.count(pos) > 0;
    });

    if (!is_annoyed) continue;

    for (const bj::Direction direction : directions) {
      // We are at east
      const auto [up, down] = travel_directions(direction);

      bj::Position check = elve;
      check.move(direction);

      // NE
      check.move(up);
      if (elves.count(check)) continue;

      // SE
      check.move(down);
      check.move(down);
      if (elves.count(check)) continue;

      // E
      check.move(up);
      if (elves.count(check)) continue;

      retval[check].emplace(elve);
      break;
    }
  }

  return retval;
}



bool ElvesInAVolcano::move_the_elves(const std::map<bj::Position, std::set<bj::Position>> & propositions) {
  bool noone_moved = true;

  for (const auto & [destination, proposing_elves] : propositions) {
    if (proposing_elves.size() == 1) {
      elves.erase(*proposing_elves.begin());
      elves.emplace(destination);
      noone_moved = false;
    }
  }

  return noone_moved;
}

size_t ElvesInAVolcano::empty_in_delimited() const {
  if (elves.empty()) return 0;

  bj::Position min = *elves.begin();
  bj::Position max = *elves.begin();

  for (const bj::Position elve : elves) {
    min.x = std::min(min.x, elve.x);
    min.y = std::min(min.y, elve.y);
    max.x = std::max(max.x, elve.x);
    max.y = std::max(max.y, elve.y);
  }

  const size_t rectangle_area = static_cast<size_t>(max.x - min.x + 1) * static_cast<size_t>(max.y - min.y + 1);
  return rectangle_area - elves.size();
}


void ElvesInAVolcano::draw_grid() {
  for (int y = -5; y != 10; ++y) {
  for (int x = -5; x != 10; ++x) {
    bj::Position pos { x, y };
    if (elves.count(pos)) {
      std::cout << '#';
    } else {
      std::cout << '.';
    }
  }
  std::cout << "\n";
  }
}

Output day_2022_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::set<bj::Position> elves;

  for (size_t y = 0; y != lines.size(); ++y) {
    const std::string & line = lines[y];
    for (size_t x = 0; x != line.size(); ++x) {
      if (line[x] == '#') {
        elves.emplace(bj::Position{ static_cast<int>(x), static_cast<int>(y) });
      }
    }
  }

  ElvesInAVolcano volcano{ elves };

  // Part A
  for (size_t i = 0; i != 10; ++i) {
    volcano.run_a_round(i);
  }

  const auto empty_at_10 = volcano.empty_in_delimited();

  // Part B
  volcano.elves = elves;
  size_t round = 0;
  while (!volcano.run_a_round(round)) {
    ++round;
  }
  ++round;

  return Output(empty_at_10, round);
}
