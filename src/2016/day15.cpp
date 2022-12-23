#include "../advent_of_code.hpp"
#include <regex>
#include <algorithm>
#include <array>

namespace {

struct Disc {
  long long int current;
  long long int max;
};

long long int solve(const std::vector<Disc> & disks) {
  long long int i = 0;
  while (true) {
    long long int disks_i = 0;
    while (disks_i < disks.size()) {
      const Disc & disc = disks[disks_i];
      if ((disc.current + disks_i + i + 1) % disc.max != 0) {
        break;
      }

      ++disks_i;
    }

    if (disks_i == disks.size()) {
      return i;
    }

    i += 1;
  }
} 

}

Output day_2016_15(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  constexpr const char * regex_str = R"(Disc #(\d) has (\d+) positions; at time=0, it is at position (\d+)\.)";
  std::regex regex_ { regex_str };
  std::smatch smatch_;

  std::vector<Disc> disks;

  for (const std::string & line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      throw std::runtime_error("agnagna");
    }

    disks.emplace_back(Disc{ std::stoi(smatch_[3].str()), std::stoi(smatch_[2].str()) });
  }

  const auto part_a = solve(disks);

  disks.emplace_back(Disc{ 0, 11 });

  const auto part_b = solve(disks);

  return Output(part_a, part_b);
}
