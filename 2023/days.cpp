#include "../common/advent_of_code.hpp"
#include "../common/configuration.h"

Output day_2023_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_09(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2023 {
  &day_2023_01, &day_2023_02, &day_2023_03, &day_2023_04, &day_2023_05,
  &day_2023_06, &day_2023_07, &day_2023_08, &day_2023_09, 
  nullptr
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2023, argc, argv);
  return 0;
}
