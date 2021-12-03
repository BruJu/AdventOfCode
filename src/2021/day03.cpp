#include "../advent_of_code.hpp"
#include <algorithm>
#include <optional>

template<typename Callable>
static inline void for_each_bit(size_t nb_of_bits, Callable callable) {
  for (size_t i = 0; i != nb_of_bits; ++i) {
    const auto as_int = static_cast<int>(nb_of_bits - i) - 1;
    const auto bit = 1 << as_int;
    callable(i, bit);
  }
}

static std::optional<int> search_rating(const std::vector<int> & values, int higher_power_of_2, bool search_most_common) {
  std::vector<int> remaing_values = values;
  int current_bit = higher_power_of_2;

  while (remaing_values.size() > 1 && current_bit != 0) {
    const size_t nb_of_values_with_current_bit = std::count_if(
      remaing_values.begin(),
      remaing_values.end(),
      [current_bit](int value) { return value & current_bit; }
    );
    
    const size_t nb_of_values_without_current_bit = remaing_values.size() - nb_of_values_with_current_bit;
    const bool most_common_is_1 = nb_of_values_with_current_bit >= nb_of_values_without_current_bit;

    remaing_values.erase(
      std::remove_if(
        remaing_values.begin(),
        remaing_values.end(),
          [&](int value) {
              const bool has_bit_on_one = value & current_bit;
              const bool is_most_common = has_bit_on_one == most_common_is_1;
              const bool is_what_we_want = is_most_common == search_most_common;
              return is_what_we_want;
          }
      ),
      remaing_values.end()
    );

    current_bit /= 2;
  }

  if (remaing_values.size() != 1) return std::nullopt;
  return remaing_values[0];
}

Output day_2021_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const int nb_of_bits = static_cast<int>(lines[0].size());

  const auto input = lines_transform::map<int>(lines, [](const std::string & as_string) {
    int as_int = 0;
    for (const auto c : as_string) {
      as_int = as_int * 2 + (c == '1' ? 1 : 0);
    }
    return as_int;
  });

  // Part A
  auto values_per_bit = std::vector<size_t>(nb_of_bits);
  
  for (const auto & line : lines) {
    for (size_t i = 0; i != line.size(); ++i) {
      if (line[i] == '1') {
        ++values_per_bit[i];
      }
    }
  }

  int gamma = 0;
  int epsilon = 0;
  const size_t half = lines.size() / 2;

  for_each_bit(nb_of_bits,
    [&](size_t i, int bit) {
      if (values_per_bit[i] > half) {
        gamma = gamma | bit;
      } else {
        epsilon = epsilon | bit;
      }
    }
  );

  // Part B
  const auto oxygen = search_rating(input, 1 << (nb_of_bits - 1), true);
  const auto co2 = search_rating(input, 1 << (nb_of_bits - 1), false);

  return Output(gamma * epsilon, *oxygen**co2);
}
