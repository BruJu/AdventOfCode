#include <iostream>
#include "framework/configuration.h"
#include "2020/days.hpp"
#include <algorithm>
#include "colors.h"
#include <map>

static auto get_all_handlers() {
    std::map<int, std::array<DayEntryPoint *, 25>> map;
    map[2020] = days_2020;
    return map;
}

template <typename Map>
static auto highest_day(const Map & m) {
    return std::max_element(m.begin(), m.end(),
            [](const auto & lhs, const auto & rhs) { return lhs.first < rhs.first; }
        )->first;
}

void print(const InputConfig & config, const std::optional<test::RunResult> & r);

void dispatch(const InputConfig & config, test::Score & ts, const std::array<DayEntryPoint *, 25> & days) {
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
        const auto t = static_cast<int>(r->elapsed_time.count() * 1000);
        std::sprintf(buffer, "%5d ms   ", t); std::cout << buffer;

        for (const auto part_result : r->parts) {
            if (!part_result) {
                std::sprintf(buffer, "%-35s ", " ");
                std::cout << buffer;
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
    const auto handlers = get_all_handlers();

    const int year = argc > 2 ? std::stoi(argv[1]) : highest_day(handlers);
    const int day  = argc > 2 ? std::stoi(argv[2]) :
                     argc > 1 ? std::stoi(argv[1]) : -2;

    const auto handlers_it = handlers.find(year);
    if (handlers_it == handlers.end()) {
        std::cerr << "Unknown year: " << year << '\n';
        return 1;
    }

    auto configs = InputConfig::read_configuration(year);

    const int required_day = day != -2 ? day : InputConfig::last_day(configs);

    test::Score testScore;

    std::optional<int> last_seen_day = std::nullopt;

    for (const auto & config : configs) {
        if (config.day == required_day || required_day == 0) {
            if (last_seen_day.has_value() && config.day != *last_seen_day) {
                std::cout << "\n";
            }
            last_seen_day = config.day;

            dispatch(config, testScore, handlers_it->second);
        }
    }

    char time[256];
    std::sprintf(time, "%02ld:%02ld:%03ld",
        std::chrono::duration_cast<std::chrono::minutes>(testScore.total_time).count(),
        std::chrono::duration_cast<std::chrono::seconds>(testScore.total_time).count() % 60,
        std::chrono::duration_cast<std::chrono::milliseconds>(testScore.total_time).count() % 1000
    );

    std::cout << "\x1B[1m"
              << "\nTotal  = " << testScore.total()
              << "\n" KGRN "Success= " << testScore.success
              << "\n" KRED "Failed = " << testScore.failed << RST << '\n'
              << "\n\x1B[1m" KCYN     "Time = " << time << RST << '\n';

    return 0;
}
