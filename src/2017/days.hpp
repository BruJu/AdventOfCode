#include "../framework/configuration.h"

Output day_2017_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2017_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2017_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2017_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2017_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2017_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2017_07(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2017 {
    &day_2017_01, &day_2017_02, &day_2017_03, &day_2017_04, &day_2017_05,
    &day_2017_06, &day_2017_07, 
    nullptr
};
