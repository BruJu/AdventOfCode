#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <optional>

static int to_int(const std::string & str) {
  int val = 0;
  for (const char c : str) {
    val = val * 2 + (c == '1' ? 1 : 0);
  }
  return val;
}

static std::optional<int> search_rating(const std::vector<std::string> & values, size_t pos, bool search_most_common) {
  if (values.empty())          return std::nullopt;
  if (values.size() == 1)      return to_int(values[0]);
  if (pos == values[0].size()) return std::nullopt;
  
  size_t number_of_1 = 0;
  for (const std::string & value : values) {
    if (value[pos] == '1') {
      ++number_of_1;
    }
  }

  const size_t number_of_0 = values.size() - number_of_1;

  const bool keep_1 = 
    (search_most_common && number_of_1 >= number_of_0)
    || (!search_most_common && number_of_1 < number_of_0)
  ;

  std::vector<std::string> filtered;
  for (const std::string & value : values) {
    if ((value[pos] == '1') == (keep_1)) {
      filtered.emplace_back(value);
    }
  }

  return search_rating(filtered, pos + 1, search_most_common);
}

Output day_2021_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const int nb_of_bits = static_cast<int>(lines[0].size());

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

  for (int i = 0; i != nb_of_bits; ++i) {
    gamma *= 2;
    epsilon *= 2;

    if (values_per_bit[i] > half) {
      gamma += 1;
    } else {
      epsilon += 1;
    }
  }

  // Part B
  const auto oxygen = search_rating(lines, 0, true );
  const auto co2    = search_rating(lines, 0, false);

  return Output(gamma * epsilon, *oxygen**co2);
}
