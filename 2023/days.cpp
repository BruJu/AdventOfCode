#include "../common/advent_of_code.hpp"
#include "../common/configuration.h"

Output day_2023_01(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2023 {
  &day_2023_01, nullptr
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2023, argc, argv);
  return 0;
}
