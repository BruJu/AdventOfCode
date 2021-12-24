#include "../framework/configuration.h"

Output day_2016_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_16(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2016 {
    &day_2016_01, &day_2016_02, &day_2016_03, &day_2016_04, nullptr,
    &day_2016_06, &day_2016_07, &day_2016_08, &day_2016_09, &day_2016_10,
    &day_2016_11, &day_2016_12, &day_2016_13, nullptr     , nullptr,
    &day_2016_16, nullptr
};
