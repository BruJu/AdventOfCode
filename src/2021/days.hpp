#include "../framework/configuration.h"

Output day_2021_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_15(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_16(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_17(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_18(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_19(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_20(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2021_21(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2021 {
    &day_2021_01, &day_2021_02, &day_2021_03, &day_2021_04, &day_2021_05,
    &day_2021_06, &day_2021_07, &day_2021_08, &day_2021_09, &day_2021_10,
    &day_2021_11, &day_2021_12, &day_2021_13, &day_2021_14, &day_2021_15,
    &day_2021_16, &day_2021_17, &day_2021_18, &day_2021_19, &day_2021_20,
    &day_2021_21,
    nullptr
};
