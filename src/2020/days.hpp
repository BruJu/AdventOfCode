#include "../framework/configuration.h"

Output day_2020_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_13(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_14(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_15(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_16(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_17(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_18(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_19(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_20(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_21(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_22(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_23(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_24(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2020_25(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2020 {
    &day_2020_01, &day_2020_02, &day_2020_03, &day_2020_04, &day_2020_05,
    &day_2020_06, &day_2020_07, &day_2020_08, &day_2020_09, &day_2020_10,
    &day_2020_11, &day_2020_12, &day_2020_13, &day_2020_14, &day_2020_15,
    &day_2020_16, &day_2020_17, &day_2020_18, &day_2020_19, &day_2020_20,
    &day_2020_21, &day_2020_22, &day_2020_23, &day_2020_24, &day_2020_25
};
