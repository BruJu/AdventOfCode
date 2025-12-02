#include "../common/advent_of_code.hpp"
#include "../common/configuration.h"

Output day_2025_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2025_02(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_03(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_04(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_05(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_06(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_07(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_08(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_09(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_10(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_11(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));
Output day_2025_12(const std::vector<std::string> & lines, const DayExtraInfo &) __attribute__ ((weak));

static constexpr std::array<DayEntryPoint *, 25> days_2025 {
  &day_2025_01, &day_2025_02, &day_2025_03, &day_2025_04, &day_2025_05,
  &day_2025_06, &day_2025_07, &day_2025_08, &day_2025_09, &day_2025_10,
  &day_2025_11, &day_2025_12,
  nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr
};

int main(int argc, const char * argv[]) {
  run_aoc(days_2025, argc, argv);
  return 0;
}
