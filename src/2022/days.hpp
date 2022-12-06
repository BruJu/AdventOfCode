#include "../framework/configuration.h"

Output day_2022_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_06(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2022 {
    &day_2022_01, &day_2022_02, &day_2022_03, &day_2022_04, &day_2022_05,
    &day_2022_06, nullptr
};

