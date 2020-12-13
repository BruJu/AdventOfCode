#include "libs.hpp"
#include <algorithm>

// https://adventofcode.com/2020/day/13

using Int = long long int;

struct Bus {
    Int id;
    Int period;

    [[nodiscard]] Int next_stop(Int now) const {
        const Int already_passed = now / period;
        if (already_passed * period == now) return now;

        return (already_passed + 1) * period;
    }
};

static std::vector<Bus> read_buses(std::string_view line) {
    std::vector<Bus> buses;

    StringSplitter splitter = StringSplitter(line, ',');
    Int id = 0;
    while (splitter) {
        std::string split = splitter();
        if (split != "x") {
            buses.emplace_back(Bus { id, std::stoll(split) });
        }

        ++id;
    }

    return buses;
}

static Int find_departure_timestamp(const Int now, const std::vector<Bus> & buses) {
    const auto my_bus = std::min_element(buses.begin(), buses.end(),
        [now](const Bus & lhs, const Bus & rhs) {
            return lhs.next_stop(now) < rhs.next_stop(now);
        }
    );

    return (my_bus->next_stop(now) - now) * my_bus->period;
}

Output day13(const std::vector<std::string> & lines) {
    const Int timestamp = std::stoll(lines[0]);
    const std::vector<Bus> buses = read_buses(lines[1]);

    for (Bus bus : buses) {
        std::cout << bus.id << ' ' << bus.period << " ; ";
    }
    std::cout << "\n";

    return Output(
        find_departure_timestamp(timestamp, buses),
        0
    );
}
