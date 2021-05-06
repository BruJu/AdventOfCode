#include "../framework/configuration.h"

Output day_2015_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_15(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_16(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_17(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_18(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_19(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_20(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_21(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_22(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_23(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_24(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2015_25(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2015 {
    &day_2015_01, &day_2015_02, &day_2015_03, nullptr, &day_2015_05,
    &day_2015_06, &day_2015_07, &day_2015_08, &day_2015_09, &day_2015_10,
    &day_2015_11, nullptr     , &day_2015_13, &day_2015_14, &day_2015_15,
    &day_2015_16, &day_2015_17, &day_2015_18, &day_2015_19, &day_2015_20,
    &day_2015_21, &day_2015_22, &day_2015_23, &day_2015_24, &day_2015_25
};
