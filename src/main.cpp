#include <iostream>
#include "framework/configuration.h"
#include <algorithm>
#include "colors.h"

using DayEntryPoint = Output(const std::vector<std::string> & lines);

Output day01(const std::vector<std::string> & lines);
Output day02(const std::vector<std::string> & lines);
Output day03(const std::vector<std::string> & lines);
Output day04(const std::vector<std::string> & lines);
Output day05(const std::vector<std::string> & lines);
Output day06(const std::vector<std::string> & lines);
Output day07(const std::vector<std::string> & lines);
Output day08(const std::vector<std::string> & lines);
Output day09(const std::vector<std::string> & lines);
Output day10(const std::vector<std::string> & lines);
Output day11(const std::vector<std::string> & lines);
Output day12(const std::vector<std::string> & lines);

void print(const InputConfig & config, const std::optional<test::RunResult> & r);

void dispatch(const InputConfig & config, test::Score & ts) {
    static constexpr std::array<DayEntryPoint *, 30> days {
        &day01, &day02, &day03, &day04, &day05, &day06, &day07,
        &day08, &day09, &day10, &day11, &day12, nullptr
    };

    if (DayEntryPoint * day = days[config.day - 1]) {
        std::optional<test::RunResult> r = config.run(day);
        print(config, r);
        ts += r;
    } else {
        print(config, std::nullopt);
    }
}

void print(const InputConfig & config, const std::optional<test::RunResult> & r) {
    // I really hate std::ostream, and std::format is not yet available in g++
    // So let's pretend it's C even thought that's unsafe

    // Macro because with a lambda, g++ is unhappy about buffer being a variable
    char buffer[512];
    // --
    test::TestValidation overall = r ? r->get_overall() : test::TestValidation::Fail;
    std::cout << "\x1B[1m" << test::get_color(overall) << "-- ";

    std::sprintf(buffer, "Day %02d %-30s ", config.day, config.filename.c_str());
    std::cout << buffer;

    if (!r) {
        std::cout << KRED << "No handler";
    } else {
        for (const auto part_result : r->parts) {
            if (!part_result) {
                std::sprintf(buffer, "%-35s ", " ");
                continue;
            }

            switch (part_result->type) {
                case test::TestValidation::Success:
                    std::sprintf(buffer, KGRN "%-35lld ", part_result->computed);
                    break;
                case test::TestValidation::Computed:
                    std::sprintf(buffer, KBLU "%-35lld ", part_result->computed);
                    break;
                case test::TestValidation::Fail:
                    std::sprintf(buffer, KRED "%-15lld EXP=%-15lld ", part_result->computed, part_result->expected);
                    break;
            }

            std::cout << buffer;
        }
    }

    std::cout << RST "\n";
}

int main(int argc, const char * argv[]) {
    auto configs = InputConfig::read_configuration();

    int required_day;
    if (argc <= 1) {
        required_day = std::max_element(configs.begin(), configs.end(),
            [](const auto & lhs, const auto & rhs) { return lhs.day < rhs.day; }
        )->day;
    } else {
        required_day = std::stoi(argv[1]);
    }

    test::Score testScore;

    std::optional<int> last_seen_day = std::nullopt;

    // TODO : display ordered by day > part instead of day > files

    for (const auto & config : configs) {
        if (config.day == required_day || required_day == 0) {
            if (last_seen_day.has_value() && config.day != *last_seen_day) {
                std::cout << "\n";
            }
            last_seen_day = config.day;

            dispatch(config, testScore);
        }
    }

    std::cout << "\x1B[1m"
              << "\nTotal  = " << testScore.total()
              << "\n" KGRN "Success= " << testScore.success
              << "\n" KRED "Failed = " << testScore.failed << RST << '\n';

    return 0;
}
