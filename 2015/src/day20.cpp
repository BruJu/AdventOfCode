#include "../../common/advent_of_code.hpp"

#include <cmath>
#include <map>
#include <algorithm>
#include "../../util/prime_decomposition.hpp"

// https://adventofcode.com/2015/day/20

//! == Part A:
//! Given houses identified by numbers from 1 to h and elves identified by numbers
//! from 1 to e:
//! The elve number e deposits 10 presents to the house number e, 2e, 3e, ...
//!
//! What is the house with the lowest number that receives at least X presents?
//! 
//! == Part B:
//! Elves now deposit 11 presents and stop after visiting 50 houses

namespace {
    /**
     * Return the number of the house with the lowest number that have at least
     * `required_present` presents deposited by the elves.
     * 
     * @param present_per_elve Number of presents deposited per elve at one house
     * @param required_presents Number of presents that the house must have
     * @param predicate A bipredicate(elve number, house number) that checks if
     * the elve actually deposits something at the house
     */
    template<int64_t present_per_elve, typename P>
    static long long int do_part_a(int64_t required_presents, P predicate) {
        Dividers dividers;

        std::vector<int64_t> presents;
        int64_t house = 0;

        int64_t gifts;
        do {
            ++house;

            if (house > 10000 && house < 820000) {
                // Speed up execution to make running all viable
                house = 820000;
                dividers.ensure_has_all_primes(house);
            }

            gifts = 0;
            
            for (const auto divider : dividers.get_dividers_of(house)) {
                if (predicate(divider, house)) {
                    gifts += divider * present_per_elve;
                }
            }
        } while (gifts < required_presents);

        return house;
    }
    
}

Output day_2015_20(const std::vector<std::string> & lines, const DayExtraInfo &) {
    const long long int input = std::stoll(lines[0]);

    const auto part_a = do_part_a<10>(input, [](int64_t, int64_t) { return true; });
    const auto part_b = do_part_a<11>(input, [](int64_t elve, int64_t house) { return house / elve < 50; });

    return Output(part_a, part_b);
}
