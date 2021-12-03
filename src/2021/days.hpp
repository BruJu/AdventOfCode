#include "../framework/configuration.h"

Output day_2021_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_03(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2021 {
    &day_2021_01, &day_2021_02, &day_2021_03, nullptr
};
