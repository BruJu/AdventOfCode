#include "../framework/configuration.h"

Output day_2015_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_07(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2015 {
    &day_2015_01, &day_2015_02, &day_2015_03, nullptr, &day_2015_05,
    &day_2015_06, &day_2015_07, nullptr
};