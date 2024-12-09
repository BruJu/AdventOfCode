#include "../common/advent_of_code.hpp"
#include "../common/configuration.h"

Output day_2024_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_15(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_16(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_17(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_18(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_19(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_20(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_21(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_22(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_23(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_24(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2024_25(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2024 {
  &day_2024_01, &day_2024_02, &day_2024_03, &day_2024_04, &day_2024_05,
  &day_2024_06, &day_2024_07, &day_2024_08, &day_2024_09, nullptr /*&day_2024_10, 
  &day_2024_11, &day_2024_12, &day_2024_13, &day_2024_14, &day_2024_15, 
  &day_2024_16, &day_2024_17, &day_2024_18, &day_2024_19, &day_2024_20, 
  &day_2024_21, &day_2024_22, &day_2024_23, &day_2024_24, &day_2024_25
  */
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2024, argc, argv);
  return 0;
}
