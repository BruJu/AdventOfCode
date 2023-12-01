#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>

// https://adventofcode.com/2022/day/6
// Find the first position where the n previous characters are distinct

template<size_t PrefixSize>
[[nodiscard]] size_t find_end_of_start_of_message(const std::string & line) {
  if (line.size() < PrefixSize) return static_cast<size_t>(-1);
  
  std::map<char, int> occurrences;

  for (size_t i = 0; i != PrefixSize - 1; ++i) {
      ++occurrences[line[i]];
  }

  for (size_t i = PrefixSize - 1; i != line.size(); ++i) {
    // Count new symbol
    ++occurrences[line[i]];

    if (occurrences.size() == PrefixSize) {
      return i + 1;
    }

    // Remove symbol out of window
    const char out_of_window_symbol = line[i - (PrefixSize - 1)];
    const auto it_occurrences = occurrences.find(out_of_window_symbol);
    --(it_occurrences->second);

    if (it_occurrences->second == 0) {
      occurrences.erase(it_occurrences);
    }
  }

  return static_cast<size_t>(-1);
}

Output day_2022_06(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  return Output(
    dei.can_skip_part_A ? 0 : find_end_of_start_of_message< 4>(lines[0]),
    dei.can_skip_part_B ? 0 : find_end_of_start_of_message<14>(lines[0])
  );
}
