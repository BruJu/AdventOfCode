#include "../common/advent_of_code.hpp"
#include "../common/configuration.h"

Output day_2019_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2019_08(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2019 {
  &day_2019_01, &day_2019_02, &day_2019_03, &day_2019_04, &day_2019_05,
  &day_2019_06, &day_2019_07, &day_2019_08, 
  nullptr
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2019, argc, argv);
  return 0;
}
