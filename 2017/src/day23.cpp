#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <cstring>
#include <array>

// https://adventofcode.com/2017/day/23

using Int = std::int64_t;

namespace {

struct ProgramState {
  int pc = 0;
  std::array<Int, 'h' - 'a' + 1> registers;

  std::uint64_t number_of_mul = 0;

  ProgramState() {
    registers.fill(0);
  }

  Int get(const std::string & str) const {
    if (str.size() == 1 && str[0] >= 'a' && str[0] <= 'h') {
      return registers[str[0] - 'a'];
    } else {
      return std::stoll(str);
    }
  }

  void set(const std::string & str, Int new_value) {
    registers[str[0] - 'a'] = new_value;
  }
};

}

std::vector<int> get_primes_until(int max) {
  std::vector<int> primes;
  primes.emplace_back(2);

  for (int i = 3; i < max; ++i) {
    auto bad = std::any_of(primes.begin(), primes.end(), [i](int prime) {
      return i % prime == 0;
    });

    if (!bad) {
      primes.emplace_back(i);
    }
  }

  return primes;
}

Output day_2017_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Part A
  ProgramState state;

  state.registers['a' - 'a'] = 1;

  while (state.pc >= 0 && std::cmp_less(state.pc, lines.size())) {
    const auto tokens = bj::string_split(lines[state.pc], " ");

    const std::string & operation = tokens[0];
    const std::string & X = tokens[1];
    const std::string & Y = tokens[2];

    if (operation == "set") {
      state.set(X, state.get(Y));
      ++state.pc;
    } else if (operation == "sub") {
      state.set(X, state.get(X) - state.get(Y));
      ++state.pc;
    } else if (operation == "mul") {
      state.set(X, state.get(X) * state.get(Y));
      ++state.number_of_mul;
      ++state.pc;
    } else if (operation == "jnz") {
      const Int val = state.get(X);
      if (val != 0) {
        state.pc += state.get(Y);
      } else {
        ++state.pc;
      }
    } else {
      throw std::runtime_error("???");
    }
  }

  // answer is (initial value of b - 2)²

  // Part B

// b = 109300 
// c = b + 17000
// while (true) {
//   if b est premier:
//     h++
//   
//   if b == c:
//     return h;
// 
//   b += 17;
// }

  const auto all_primes = get_primes_until(109300 + 17000);

  int primes = 0;
  for (int b = 109300; b <= 109300 + 17000; b += 17) {
    if (std::find(all_primes.begin(), all_primes.end(), b) == all_primes.end()) {
      ++primes;
    }
  }

  return Output(state.number_of_mul, primes);
}
