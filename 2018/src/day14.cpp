#include "../../common/advent_of_code.hpp"
#include <span>

// https://adventofcode.com/2018/day/14

struct RecipeeGame {
  size_t elve_1 = 0;
  size_t elve_2 = 1;
  std::vector<int> recipees;

  RecipeeGame() {
    recipees.emplace_back(3);
    recipees.emplace_back(7);
  }

  void run() {
    int sum = recipees[elve_1] + recipees[elve_2];

    if (sum >= 10) {
      recipees.emplace_back(sum / 10);
      recipees.emplace_back(sum % 10);
    } else {
      recipees.emplace_back(sum);
    }

    elve_1 = (elve_1 + recipees[elve_1] + 1) % recipees.size();
    elve_2 = (elve_2 + recipees[elve_2] + 1) % recipees.size();
  }

};


std::string run_part_a(int nb_of_recipees) {
  RecipeeGame recipees;

  while (std::cmp_less(recipees.recipees.size(), nb_of_recipees + 10)) {
    recipees.run();
  }

  std::string answer = "";
  for (int i = 0; i != 10; ++i) {
    answer += std::to_string(recipees.recipees[nb_of_recipees + i]);
  }
  return answer;
}


std::int64_t run_part_b(std::span<const int> target) {
  RecipeeGame recipees;
  
  std::int64_t next_to_check = 0;
  while (true) {
    while (next_to_check + target.size() >= recipees.recipees.size()) {
      recipees.run();
    }

    while (next_to_check + target.size() < recipees.recipees.size()) {
      size_t k = 0;
      while (k < target.size()
        && recipees.recipees[next_to_check + k] == target[k]) {
        ++k;
      }

      if (k == target.size()) return next_to_check;

      ++next_to_check;
    }
  }
}

Output day_2018_14(const std::vector<std::string> &, const DayExtraInfo & dei) {
  const std::string part_a = run_part_a(dei.part_a_extra_param);
  
  std::vector<int> searched;
  int val = dei.part_b_extra_param;
  while (val != 0 || searched.size() < 5) {
    searched.insert(searched.begin(), val % 10);
    val /= 10;
  }

  const std::int64_t part_b = run_part_b(searched);

  return Output(part_a, part_b);
}
