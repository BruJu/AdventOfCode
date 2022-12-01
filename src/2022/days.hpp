#include "../framework/configuration.h"

Output day_2022_01(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2022 {
    &day_2022_01, nullptr
};
