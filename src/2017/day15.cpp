#include "../advent_of_code.hpp"
#include <vector>

// https://adventofcode.com/2017/day/15


struct Generator {
  std::uint64_t current_value;
  std::uint64_t factor;
  std::uint64_t multiples_of;

  Generator(std::uint64_t initial_value, std::uint64_t factor, std::uint64_t multiples_of = 1)
    : current_value(initial_value), factor(factor), multiples_of(multiples_of) {}

  std::int32_t next() {
    do {
      current_value *= factor;
      current_value %= 0x7FFFFFFF;
    } while (current_value % multiples_of != 0);

    return static_cast<std::int32_t>(current_value);
  }
};

size_t match(Generator gen_a, Generator gen_b, int bound) {
  int total = 0;
  for (int i = 0; i != bound; ++i) {
    const auto a = gen_a.next() & 0xFFFF;
    const auto b = gen_b.next() & 0xFFFF;

    if (a == b) ++total;
  }

  return total;
}


Output day_2017_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Generator generator_a ( bj::extract_ints_from_line(lines[0])[0], 16807 );
  Generator generator_b ( bj::extract_ints_from_line(lines[1])[0], 48271 );

  const auto part_a = match(generator_a, generator_b, 40000000);
  
  generator_a.multiples_of = 4;
  generator_b.multiples_of = 8;

  const auto part_b = match(generator_a, generator_b, 5000000);

  return Output(part_a, part_b);
}
