#include "libs.hpp"
#include <algorithm>

// https://adventofcode.com/2020/day/13

// https://en.wikipedia.org/wiki/Chinese_remainder_theorem
// I figured out myself that it was related to congruences... the algorithm
// is not easy to implement and get it right if you don't know it already,
// and to me, Advent of Code is a quick exercice everyday, so I'm going to
// pass on it atm.

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

static bool is_contiguous_start(Int now, const std::vector<Bus> & buses) {
    return std::all_of(buses.begin(), buses.end(),
        [now](const Bus & bus) {
            return (now + bus.id) % bus.period == 0;
        }
    );
}

[[maybe_unused]] static Int find_continuous_bruteforce(const std::vector<Bus> & buses) {
    Int step = buses[0].id == 0 ? buses[0].period : 1;
    Int now = 0;

    while (!is_contiguous_start(now, buses)) {
        now += step;

        if (now >= 1300000) {
            return 0;
        }
    }

    return now;
}

[[maybe_unused]] static Int wolframalpha(const std::vector<Bus> & buses) {
    struct Mods {
        Int remainder;
        Int mod;
    };

    std::vector<Mods> mods;
    for (const auto & bus : buses) {
        mods.push_back(Mods { bus.id, bus.period });
        std::printf("(n+%lld)%%%lld = 0,", bus.id, bus.period);
    }
    std::printf("\n");
    std::printf("https://www.wolframalpha.com\n");

    return 0;
}

[[maybe_unused]] static Int chinese_remainder(const std::vector<Bus> & buses) {
    // Doesn't work
    struct Mods {
        Int remainder;
        Int mod;
    };

    std::vector<Mods> mods;
    for (const auto & bus : buses) {
        mods.push_back(Mods { bus.id, bus.period });
        std::printf("(n+%lld)%%%lld = 0,", bus.id, bus.period);
    }

    //mods.push_back(Mods{ 2, 3 });
    //mods.push_back(Mods{ 3, 5 });
    //mods.push_back(Mods{ 2, 7 });

    Int product = 1;
    for (const auto & mod : mods) {
        product *= mod.mod;
    }

    Int x = 0;

    for (auto & mod : mods) {
        std::cout << "n = " << mod.mod;
        Int hatted_mod = product / mod.mod;
        std::cout << " ^= " << hatted_mod;

        Int times_hatted = 1;
        while (hatted_mod * times_hatted % mod.mod != 1) {
            ++times_hatted;
        }
        std::cout << "times= " << times_hatted;

        Int e = times_hatted * hatted_mod;
        std::cout << "e= " << e;

        x += mod.remainder * e;
        std::cout << "\n";
    }

    std::cout << x << "\n";
    std::cout << x % product << "\n";

    return x % product;

}

Output day13(const std::vector<std::string> & lines) {
    const Int timestamp = std::stoll(lines[0]);
    const std::vector<Bus> buses = read_buses(lines[1]);

    return Output(
        find_departure_timestamp(timestamp, buses),
        0
    );
}
