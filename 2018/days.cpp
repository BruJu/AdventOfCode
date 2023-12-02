#include "../common/advent_of_code.hpp"
#include "../common/configuration.h"

Output day_2018_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2018_15(const std::vector<std::string> & lines, const DayExtraInfo &);


static constexpr std::array<DayEntryPoint *, 25> days_2018 {
  &day_2018_01, &day_2018_02, &day_2018_03, &day_2018_04, &day_2018_05, 
  &day_2018_06, &day_2018_07, &day_2018_08, &day_2018_09, &day_2018_10,
  &day_2018_11, &day_2018_12, &day_2018_13, &day_2018_14, &day_2018_15,
  nullptr
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2018, argc, argv);
  return 0;
}
