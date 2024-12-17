#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <set>

// https://adventofcode.com/2024/day/13

namespace {

bool is_too_far(bj::Position claw, bj::Position target) {
  return claw.x > target.x || claw.y > target.y;
}

struct ClawMachine {
  bj::Position a_button;
  bj::Position b_button;
  bj::Position prize;

  ClawMachine(
    std::string line1,
    std::string line2,
    std::string line3
  ) {

    const auto v1 = bj::extract_ints_from_line(line1);
    const auto v2 = bj::extract_ints_from_line(line2);
    const auto v3 = bj::extract_ints_from_line(line3);

    a_button = { v1[0], v1[1] };
    b_button = { v2[0], v2[1] };
    prize    = { v3[0], v3[1] };
  }

  int solve_a() const {
    bj::Position a_button = this->a_button;
    bj::Position b_button = this->b_button;

    bj::Position current_position = bj::Position{ 0, 0 };

    std::set<int> min_value;

    int hit_a = 0;
    int hit_b = 0;

    while (!is_too_far(current_position + a_button, prize)) {
      current_position += a_button;
      hit_a += 1;
    }

    const auto hit_the_b_button = [&]() {
      while (!is_too_far(current_position + b_button, prize)) {
        current_position += b_button;
        hit_b += 1;
      }
    };

    while (hit_a >= 0) {
      hit_the_b_button();

      if (current_position == prize && hit_a <= 100 && hit_b <= 100) {
        min_value.emplace(hit_a * 3 + hit_b);
      }

      current_position -= hit_b * b_button;
      hit_b = 0;

      --hit_a;
      current_position -= a_button;
    }

    return min_value.empty() ? 0 : *min_value.begin();
  }

};




}

Output day_2024_13(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<ClawMachine> claw_machines;
  for (size_t i = 0; i < lines.size(); i += 4) {
    claw_machines.emplace_back(lines[i], lines[i + 1], lines[i + 2]);
  }

  long long int part_a = 0;
  for (const ClawMachine & claw_machine : claw_machines) {
    part_a += claw_machine.solve_a();
  }

  return Output(part_a, "run js/2024_13.py"); 
}
