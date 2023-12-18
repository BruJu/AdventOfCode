#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include "../../util/position.hpp"
#include "../../util/instruction_reader.hpp"

using i64 = std::int64_t;
using Grid = std::vector<std::string>;

#pragma GCC diagnostic ignored "-Wsign-compare"

// https://adventofcode.com/2023/day/18

namespace {

struct Instruction {
  bj::Direction direction;
  int meters;

private:
  Instruction() = default;

public:
  [[nodiscard]] static Instruction Part_A(bj::Direction dir, int meters) {
    Instruction retval;
    retval.direction = dir;
    retval.meters = meters;
    return retval;
  }

  [[nodiscard]] static Instruction Part_B(const std::string & hexa) {
    Instruction retval;
    retval.direction = *bj::to_direction_from_lrtd(hexa[5], "2031");
    retval.meters = 0;
    for (size_t i = 0; i != 5; ++i) {
      int digit = (hexa[i] >= 'a' && hexa[i] <= 'f') ? (10 + hexa[i] - 'a') : (hexa[i] - '0');
      retval.meters = retval.meters * 16 + digit;
    }
    return retval;
  }
};


// Use a flood algorithm to find the outer of the digged lagoon
std::set<bj::Position> find_not_digged(const std::set<bj::Position> & trenches, const bj::Rectangle & area) {
  std::set<bj::Position> not_digged;
  std::queue<bj::Position> to_explore;
  to_explore.emplace(bj::Position{ area.left - 1, area.top - 1 });
  not_digged.emplace(bj::Position{ area.left - 1, area.top - 1 });

  while (!to_explore.empty()) {
    bj::Position pos = to_explore.front();
    to_explore.pop();

    pos.for_each_neighbour([&](bj::Position other) {
      if (other.x < area.left - 1) return;
      if (other.x > area.right + 1) return;
      if (other.y < area.top - 1) return;
      if (other.y > area.bottom + 1) return;

      if (trenches.contains(other)) return;
      if (not_digged.contains(other)) return;

      not_digged.emplace(other);
      to_explore.emplace(other);
    });
  }
  return not_digged;
}

// Draw the lagoon from the list of non digged position (ie the result of find_not_digged)
[[maybe_unused]] void draw(const std::set<bj::Position> & not_digged, const bj::Rectangle & rectangle) {
  for (int y = rectangle.top - 1; y <= rectangle.bottom + 1; ++y) {
    for (int x = rectangle.left - 1; x <= rectangle.right + 1; ++x) {
      if (not_digged.contains(bj::Position{ x, y })) {
        std::cout << '.';
      } else {
        std::cout << '#';
      }

    }
    std::cout << '\n';
  }
}

// Part A main function
i64 solve_with_regular_grid(const std::vector<Instruction> & instructions) {
  std::set<bj::Position> trenches;
  bj::Rectangle rectangle(0, 0, 0, 0);

  // Make trenches
  {
    bj::Kitten kitty { bj::Position{ 0, 0 }, bj::Direction::Left };

    for (const Instruction & instruction : instructions) {
      kitty.direction = instruction.direction;

      trenches.emplace(kitty.position);

      for (int i = 0; i != instruction.meters; ++i) {
        kitty.advance();
        trenches.emplace(kitty.position);

        rectangle.left = std::min(rectangle.left, kitty.position.x);
        rectangle.right = std::max(rectangle.right, kitty.position.x);
        rectangle.top = std::min(rectangle.top, kitty.position.y);
        rectangle.bottom = std::max(rectangle.bottom, kitty.position.y);
      }
    }
  }

  // Find outter of the lagoon
  const std::set<bj::Position> not_digged = find_not_digged(trenches, rectangle);

  // Compute the lagoon area
  return (rectangle.right - rectangle.left + 1 + 2) * (rectangle.bottom - rectangle.top + 1 + 2)
    - not_digged.size();
}


// A virtual axis of coordinates.
struct VirtualAxis {
  std::vector<int> coordinates; // always sorted

  VirtualAxis() { coordinates.emplace_back(0); }

  // Add a new coordinate
  void add(int position) {
    const auto it = std::find(coordinates.begin(), coordinates.end(), position);
    if (it != coordinates.end()) return;

    coordinates.emplace_back(position);
    std::sort(coordinates.begin(), coordinates.end());
  }

  // Return all values from min to max. min and max must be in the virtual axis.
  [[nodiscard]] std::span<const int> get_all_between(int min, int max) const {
    if (max < min) return get_all_between(max, min);

    size_t i_min = 0;
    while (coordinates[i_min] != min) {
      ++i_min;
    }

    size_t i_max = i_min;
    while (coordinates[i_max] != max) {
      ++i_max;
    }

    return std::span(coordinates.data() + i_min, i_max - i_min + 1);
  }

  // Return the position in the list of virtual coordinate of value
  [[nodiscard]] size_t get_index(int value) const {
    for (size_t i = 0; i != coordinates.size(); ++i) {
      if (coordinates[i] == value) return i;
    }
    throw std::runtime_error("bad get_index call");
  }

  // Return the width of the virtual coordinate
  //
  // ie if there are the virtual coordinates 0 1 2 5
  // The width of 0 is 1
  // The width of 2 is 3 (because 3 and 4 are missing)
  // assert(i + 1 < coordinates.size()
  [[nodiscard]] i64 get_width(size_t i) const {
    return coordinates[i + 1] - coordinates[i];
  }
};

// Part B main function
i64 solve_with_virtual_axises(const std::vector<Instruction> & instructions) {
  VirtualAxis axis_x;
  VirtualAxis axis_y;

  const auto register_position = [&](bj::Kitten & kitty) {
    for (bj::Direction dir : { bj::Direction::Left, bj::Direction::Right, bj::Direction::Down, bj::Direction::Top }) {
      kitty.direction = dir;
      kitty.advance();
      axis_x.add(kitty.position.x);
      axis_y.add(kitty.position.y);
      kitty.turn_180();
      kitty.advance();
    }
    
    axis_x.add(kitty.position.x);
    axis_y.add(kitty.position.y);
  };

  const auto move_fast = [&](bj::Kitten & kitty, int meters) {
    switch (kitty.direction) {
      case bj::Direction::Left:   kitty.position.x -= meters; break;
      case bj::Direction::Right:  kitty.position.x += meters; break;
      case bj::Direction::Top:    kitty.position.y -= meters; break;
      case bj::Direction::Down:   kitty.position.y += meters; break;
    }
  };

  // Fill the virtual axises
  {
    bj::Kitten kitty { bj::Position{ 0, 0 }, bj::Direction::Left };

    for (const Instruction & instruction : instructions) {
      register_position(kitty);

      kitty.direction = instruction.direction;
      move_fast(kitty, instruction.meters);

      register_position(kitty);
    }
  }

  // Make trenches (only on position that have a corresponding virtual coordinates)
  std::set<bj::Position> trenches;
  {
    bj::Kitten kitty { bj::Position{ 0, 0 }, bj::Direction::Left };

    for (const Instruction & instruction : instructions) {
      const bj::Position from = kitty.position;
      kitty.direction = instruction.direction;
      move_fast(kitty, instruction.meters);
      const bj::Position to = kitty.position;

      if (from.x != to.x) {
        for (const int x : axis_x.get_all_between(from.x, to.x)) {
          trenches.emplace(bj::Position{ x, from.y });
        }
      } else {
        for (const int y : axis_y.get_all_between(from.y, to.y)) {
          trenches.emplace(bj::Position{ from.x, y });
        }
      }
    }
  }
  
  // Put trenches on virtual coordinates
  std::set<bj::Position> virtual_trenches;
  {
    for (const auto real_trench : trenches) {
      const size_t virtual_x = axis_x.get_index(real_trench.x);
      const size_t virtual_y = axis_y.get_index(real_trench.y);
      virtual_trenches.emplace(bj::Position{ static_cast<int>(virtual_x), static_cast<int>(virtual_y) });
    }
  }
  
  // Flood to solve the virtual lagoon
  const bj::Rectangle area(0, 0, static_cast<int>(axis_x.coordinates.size()), static_cast<int>(axis_y.coordinates.size()));
  std::set<bj::Position> not_digged = find_not_digged(virtual_trenches, area);

  // Use virtual reverse-lagoon and virtual axis to compute the lagoon size
  i64 answer = 0;

  for (int x = 0; x <= static_cast<int>(axis_x.coordinates.size()); ++x) {
    for (int y = 0; y <= static_cast<int>(axis_y.coordinates.size()); ++y) {
      if (!not_digged.contains(bj::Position{ x, y })) {
        answer += axis_x.get_width(x) * axis_y.get_width(y);
      }
    }
  }

  return answer;
}

}

Output day_2023_18(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Instruction> instructions_part_a;
  std::vector<Instruction> instructions_part_b;

  for (const std::string & line : lines) {
    const auto part = bj::string_split(line, " ");

    instructions_part_a.emplace_back(
      Instruction::Part_A(
        bj::to_direction_from_lrtd(part[0][0], "LRUD").value(),
        std::stoi(part[1])
      )
    );

    instructions_part_b.emplace_back(Instruction::Part_B(part[2].substr(2, 6)));
  }

  const i64 part_a = solve_with_regular_grid(instructions_part_a);
  const i64 part_b = solve_with_virtual_axises(instructions_part_b);

  return Output(part_a, part_b);
}
