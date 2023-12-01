#include "../../common/advent_of_code.hpp"
#include <regex>

// https://adventofcode.com/2020/day/2

struct PolicyPassword {
    size_t first;
    size_t second;
    char key;
    std::string password;

    [[nodiscard]] static PolicyPassword from_string(const std::string & str);
};

PolicyPassword PolicyPassword::from_string(const std::string & str) {
    constexpr const char * const PATTERN = R"(^([0-9]+)-([0-9]+) ([a-zA-Z]): ([a-z0-9A-Z]*)$)";
    std::regex pattern { PATTERN };

    std::smatch matches;
    const bool rs = std::regex_search(str, matches, pattern);

    if (!rs) {
        std::cerr << "Invalid line\n" << str << "\n";
        exit(EXIT_FAILURE);
    }

    return PolicyPassword {
        .first    = std::stoul(matches[1].str()),
        .second   = std::stoul(matches[2].str()),
        .key      = matches[3].str()[0],
        .password = matches[4].str()
    };
}

static bool match_ruleset1(const PolicyPassword & pp) {
    const size_t count = std::count(
        pp.password.begin(), pp.password.end(), pp.key
    );
    return count >= pp.first && count <= pp.second;
}

static bool match_ruleset2(const PolicyPassword & pp) {
    const auto check_position = [&](const size_t position) {
        return position - 1 < pp.password.size()
            && pp.password[position - 1] == pp.key;
    };

    return check_position(pp.first) ^ check_position(pp.second);
}

Output day_2020_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const std::vector<PolicyPassword> policies = 
        lines_transform::map<PolicyPassword>(lines, PolicyPassword::from_string);

    constexpr auto count_valid = [](const auto & policies, auto predicate) {
        return static_cast<int>(std::count_if(policies.begin(), policies.end(), predicate));
    };

    const int ruleset1 = count_valid(policies, match_ruleset1);
    const int ruleset2 = count_valid(policies, match_ruleset2);

    return Output(ruleset1, ruleset2);
}
