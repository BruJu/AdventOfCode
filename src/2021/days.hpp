#include "../framework/configuration.h"

Output day_2021_01(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2021 {
    &day_2021_01, nullptr
};
