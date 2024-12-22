#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>
#include <variant>
#include <set>

// https://adventofcode.com/2024/day/22

using i64 = std::int64_t;

namespace {

i64 mix_prune(i64 a, i64 b) {
  return (a ^ b) % 16777216;
}

int generate_next(i64 value) {
  const i64 a = mix_prune(value * 64, value);
  const i64 b = mix_prune(a / 32, a);
  return mix_prune(b * 2048, b);
}

}

Output day_2024_22(const std::vector<std::string> & lines, const DayExtraInfo & dei) {

  i64 part_a = 0;

  i64 sn = 123;
  for (int i = 0; i != 10; ++i) {
    printf("%ld\n", sn);
    sn = generate_next(sn);
  }

    std::map<
      std::tuple<int, int, int, int>,
      int
    > 
  changes;

  for (const std::string & line : lines) {
    std::set<std::tuple<int, int, int, int>> seen;

    i64 secret_number = std::stoi(line);
    
    std::vector<int> loc_changes;

    for (int i = 0; i != 2000; ++i) {
      int new_secret_number = generate_next(secret_number);

      if (loc_changes.size() == 4) {
        loc_changes.erase(loc_changes.begin());
      }

      const int banana_before = secret_number % 10;
      const int banana_after = new_secret_number % 10;
      const int diff = banana_after - banana_before;

      loc_changes.emplace_back(diff);
      
      if (loc_changes.size() == 4) {
        std::tuple<int, int, int, int> tuple{
          loc_changes[0],loc_changes[1],loc_changes[2],loc_changes[3]
        };

        if (!seen.contains(tuple)) {
          changes[tuple] += banana_after;
          seen.emplace(tuple);
        }
      }

      secret_number = new_secret_number;
    }

    part_a += secret_number;
  }

  int part_b = 0;
  for (const auto & [_, gain] : changes) {
    if (gain > part_b) {
      part_b = gain;
    }
  }

  return Output(
    part_a, part_b
    );
}
