#pragma once

#include <stdint.h>
#include <vector>

/** A divider in a prime decomposition */
struct Divider {
  int64_t value;
  int64_t power;
};

/** A class that search for all dividers using prime numbers */
class Dividers {
  public:
  std::vector<int64_t> m_primes;
  int64_t m_last_seen_maybe_prime = 1;

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
  void ensure_has_all_primes(int64_t number) {
    if (m_last_seen_maybe_prime == 1) {
      add_to_prime_list_if_prime(2);
      m_last_seen_maybe_prime = 2;
    }

    for (int64_t i = m_last_seen_maybe_prime ; i <= number ; ++i) {
      if (i % 2 == 0) continue;
      add_to_prime_list_if_prime(i);
    }

    m_last_seen_maybe_prime = number + 1;
  }

  /**
   * Return the prime decomposition of number.
   * 20 = 2^2 * 5^1 -> (2, 2), (5, 1)
   * 
   * Assumes that all the prime numbers until `number` excluded are known.
   */
  std::vector<Divider> get_prime_decomposition(int64_t number) {
    if (number == 1) return {};
    ensure_has_all_primes(number);

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

private:
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

public:
  // Return all dividers of numbers
  // 20 -> 1, 2, 4, 5, 10, 20
  std::vector<int64_t> get_dividers_of(int64_t number) {
    const auto prime_dividers = get_prime_decomposition(number);

    std::vector<int64_t> dividers;
    add_dividers_rec(dividers, prime_dividers, 0, 1);
    return dividers;
  }
};
