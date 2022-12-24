#include "../framework/configuration.h"

Output day_2016_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_15(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_16(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_17(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_18(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_19(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_20(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_21(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_22(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2016_23(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2016 {
    &day_2016_01, &day_2016_02, &day_2016_03, &day_2016_04, &day_2016_05,
    &day_2016_06, &day_2016_07, &day_2016_08, &day_2016_09, &day_2016_10,
    &day_2016_11, &day_2016_12, &day_2016_13, &day_2016_14, &day_2016_15,
    &day_2016_16, &day_2016_17, &day_2016_18, &day_2016_19, &day_2016_20,
    &day_2016_21, &day_2016_22, &day_2016_23, 
    nullptr
};
