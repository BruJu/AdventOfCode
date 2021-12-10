#include "../framework/configuration.h"

Output day_2021_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_10(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2021 {
    &day_2021_01, &day_2021_02, &day_2021_03, &day_2021_04, &day_2021_05,
    &day_2021_06, &day_2021_07, &day_2021_08, &day_2021_09, &day_2021_10,
    nullptr
};
