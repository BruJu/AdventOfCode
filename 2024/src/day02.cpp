#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>

// https://adventofcode.com/2024/day/2

static
bool is_safe(std::span<const int> values) {
  const bool isIncrease = values[0] < values[1];

  for (size_t i = 0; i != values.size() - 1; ++i) {
    int diff = values[i + 1] - values[i];

    if (!isIncrease) diff = -diff;

    if (diff < 1 || diff > 3) {
      return false;
    }
  }

  return true;
}

static
bool is_tolerating_safe(const std::vector<int> & values) {
  // No values = safe
  // One value = safe
  // Two values = remove one, it's safe
  if (values.size() <= 2) return true;

  if (is_safe(values)) return true;

  // values = 0 1 2 3 4 5
  std::vector<int> copy = values;
  copy.erase(copy.begin());
  // copy = 1 2 3 4 5
  // All values are -1
  // = All values after 0 included are -1
  // i = 0

  for (size_t i = 0; i != values.size(); ++i) {
    if (is_safe(copy)) return true;

    copy[i] = values[i];
    // i=0 -> copy = 0 2 3 4 5
    // i=1 -> copy = 0 1 3 4 5
    // ...
    // All values before i+1 excluded are ok
    // All values after  i+1 included are -1.
  }

  // Possible improvments:
  // If all values up to n are "safe" in the original array,
  // Then all values up to n - 2 are always safe
  // Revert the table, same thing.
  // So we can narrow the part to actually examinate to only a few
  // values.
  // But it is useless to implement it because there are few values in the actual
  // data

  return false;
}


Output day_2024_02(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;
  long long int part_b = 0;

  for (const std::string & line : lines) {
    std::vector<int> values = StringSplitter::line_to_vector<int>(
      line,
      [](const std::string & s) { return std::stoi(s); },
      ' '
      );

    if (is_safe(values)) {
      ++part_a;
    }

    if (is_tolerating_safe(values)) {
      ++part_b;
    }
  }

  return Output(part_a, part_b);
}
