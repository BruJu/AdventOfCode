#include "../framework/configuration.h"

Output day_2015_01(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2015 {
    &day_2015_01, nullptr
};
