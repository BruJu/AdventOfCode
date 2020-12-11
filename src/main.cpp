#include <iostream>
#include "framework/configuration.h"
#include <algorithm>

namespace day01 { Output start(const std::vector<std::string> & lines); }
namespace day02 { Output start(const std::vector<std::string> & lines); }
                  Output day03(const std::vector<std::string> & lines);
                  Output day04(const std::vector<std::string> & lines);


void dispatch(const InputConfig & config) {
    switch (config.day) {
        case  1: config.run(day01::start); return;
        case  2: config.run(day02::start); return;
        case  3: config.run(day03       ); return;
        case  4: config.run(day04       ); return;



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

    for (const auto & config : configs) {
        if (config.day == required_day || required_day == 0) {
            // std::cout << config.to_string() << "\n";
            dispatch(config);
        }
    }

    return 0;
}
