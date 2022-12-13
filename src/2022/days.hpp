#include "../framework/configuration.h"

Output day_2022_01(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_02(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_03(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_04(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_05(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_06(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_07(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_08(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_09(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_10(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_11(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_12(const std::vector<std::string> & lines, const DayExtraInfo &);
Output day_2022_13(const std::vector<std::string> & lines, const DayExtraInfo &);

static constexpr std::array<DayEntryPoint *, 25> days_2022 {
    &day_2022_01, &day_2022_02, &day_2022_03, &day_2022_04, &day_2022_05,
    &day_2022_06, &day_2022_07, &day_2022_08, &day_2022_09, &day_2022_10,
    &day_2022_11, &day_2022_12, &day_2022_13, nullptr
};

