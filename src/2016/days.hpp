#include "../framework/configuration.h"

Output day_2016_01(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2016 {
    &day_2016_01,
    nullptr
};
