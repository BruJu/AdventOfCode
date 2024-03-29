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
Output day_2023_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_15(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_16(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_17(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_18(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_19(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_20(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_21(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_22(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_23(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_24(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2023_25(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2023 {
  &day_2023_01, &day_2023_02, &day_2023_03, &day_2023_04, &day_2023_05,
  &day_2023_06, &day_2023_07, &day_2023_08, &day_2023_09, &day_2023_10, 
  &day_2023_11, &day_2023_12, &day_2023_13, &day_2023_14, &day_2023_15, 
  &day_2023_16, &day_2023_17, &day_2023_18, &day_2023_19, &day_2023_20, 
  &day_2023_21, &day_2023_22, &day_2023_23, &day_2023_24, &day_2023_25
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2023, argc, argv);
  return 0;
}
