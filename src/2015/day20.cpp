#include "../advent_of_code.hpp"

#include <cmath>
#include <map>
#include <algorithm>

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
    /** A divider in a prime decomposition */
    struct Divider {
        int64_t value;
        int64_t power;
    };

    /** A class that search for dividers using prime numbers */
    class Dividers {
        std::vector<int64_t> m_primes;

        /**
         * Add number to the list of known primes if it is a number
         * 
         * Assumes that every prime number lower than `number` is known.
         */
        void add_to_prime_list_if_prime(int64_t number) {
            // Check if number is a prime
            for (const auto prime : m_primes) {
                if (number % prime == 0) {
                    return;
                }
            }

            // number is prime
            m_primes.push_back(number);
        }

    public:
        /**
         * Return the prime decomposition of number.
         * 
         * Assumes that all the prime numbers until `number` excluded are known.
         */
        std::vector<Divider> get_prime_decomposition(int64_t number) {
            if (number == 1) return {};

            add_to_prime_list_if_prime(number);

            std::vector<Divider> dividers;

            size_t index_current_prime = 0;

            while (number != 1) {
                const auto prime = m_primes[index_current_prime];
                int64_t power = 0;

                while (number % prime == 0) {
                    number /= prime;
                    ++power;
                }

                if (power != 0) {
                    dividers.push_back(Divider{ prime, power });
                }

                ++index_current_prime;
            }

            return dividers;
        }

        void add_dividers_rec(
            std::vector<int64_t> & result,
            const std::vector<Divider> & prime_decomposition,
            size_t to_explore,
            int64_t current_value
        ) {
            if (to_explore == prime_decomposition.size()) {
                result.push_back(current_value);
            } else {
                const auto & divider = prime_decomposition[to_explore];
                int64_t v = 1;
                for (int64_t power = 0 ; power <= divider.power ; ++power) {
                    add_dividers_rec(result, prime_decomposition, to_explore + 1, current_value * v);
                    v *= divider.value;
                }
            }
        }
        
        std::vector<int64_t> get_dividers_of(int64_t number) {
            const auto prime_dividers = get_prime_decomposition(number);

            std::vector<int64_t> dividers;
            add_dividers_rec(dividers, prime_dividers, 0, 1);
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
