#include <iostream>
#include "framework/configuration.h"
#include <algorithm>
#include "colors.h"

namespace day01 { Output start(const std::vector<std::string> & lines); }
namespace day02 { Output start(const std::vector<std::string> & lines); }
                  Output day03(const std::vector<std::string> & lines);
                  Output day04(const std::vector<std::string> & lines);
                  Output day05(const std::vector<std::string> & lines);


void dispatch(const InputConfig & config, TestScore & ts) {
    switch (config.day) {
        case  1: ts += config.run(day01::start); return;
        case  2: ts += config.run(day02::start); return;
        case  3: ts += config.run(day03       ); return;
        case  4: ts += config.run(day04       ); return;
        case  5: ts += config.run(day05       ); return;



        default:
            std::cout << "No program for day " << config.day << "\n";
            break;
    }
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

    TestScore testScore;

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
