#include "../advent_of_code.hpp"

#include <cmath>
#include <map>
#include <algorithm>

// https://adventofcode.com/2015/day/20

namespace {
    class Dividers {
        std::vector<int64_t> primes;

        void ensure_have_all_primes(int64_t number) {
            // number is not prime
            for (const auto prime : primes) {
                if (number % prime == 0) {
                    return;
                }
            }

            // number is prime
            primes.push_back(number);
        }

    public:
        std::vector<int64_t> get_prime_dividers_of(int64_t number) {
            if (number == 1) return {};

            ensure_have_all_primes(number);

            std::vector<int64_t> dividers;

            size_t index_current_prime = 0;

            while (number != 1) {
                if (number % primes[index_current_prime] == 0) {
                    number /= primes[index_current_prime];
                    dividers.push_back(primes[index_current_prime]);
                } else {
                    ++index_current_prime;
                }
            }

            return dividers;
        }

        void add_dividers_rec(
            std::vector<int64_t> & result,
            const std::vector<int64_t> & prime_decomposition,
            size_t to_explore,
            int64_t current_value
        ) {
            if (to_explore == prime_decomposition.size()) {
                result.push_back(current_value);
            } else {
                add_dividers_rec(result, prime_decomposition, to_explore + 1, current_value * prime_decomposition[to_explore]);
                add_dividers_rec(result, prime_decomposition, to_explore + 1, current_value);
            }
        }
        
        std::vector<int64_t> get_dividers_of(int64_t number) {
            const auto prime_dividers = get_prime_dividers_of(number);

            std::vector<int64_t> dividers;
            dividers.push_back(1);

            add_dividers_rec(dividers, prime_dividers, 0, 1);

            std::sort(dividers.begin(), dividers.end());
            const auto it = std::unique(dividers.begin(), dividers.end());
            dividers.erase(it, dividers.end());

            return dividers;
        }
    };

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

        int64_t old_max = 0;

        std::vector<int64_t> presents;

        int64_t house = 0;

        int64_t gifts;
        do {
            ++house;

            gifts = 0;

            for (const auto divider : dividers.get_dividers_of(house)) {
                if (predicate(divider, house)) {
                    gifts += divider * present_per_elve;
                }
            }

            if (old_max < gifts) {
                std::cerr << " House #" << house << ": " << gifts << "\n";

                old_max = gifts;
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
