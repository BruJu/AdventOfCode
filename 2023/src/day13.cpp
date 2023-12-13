#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/13

// For Part A
struct RowSymmetric {
  [[nodiscard]] bool operator()(std::span<const std::string> lines, size_t row) const {
    size_t left = row;
    size_t right = row + 1;

    while (right < lines.size()) {
      if (lines[left] != lines[right]) return false;

      if (left == 0) return true;
      --left;
      ++right;
    }

    return true;
  }
};


// For Part B
struct OffByOneSymmetric {
  [[nodiscard]] bool operator()(std::span<const std::string> lines, size_t row) const {
    std::optional<std::pair<size_t, size_t>> error;

    size_t left = row;
    size_t right = row + 1;

    while (right < lines.size()) {
      if (lines[left] != lines[right]) {
        if (error) return false;

        error = std::pair(left, right);
      }

      if (left == 0) break;
      --left;
      ++right;
    }

    if (!error) return false;

    const std::string & left_str  = lines[error->first];
    const std::string & right_str = lines[error->second];

    bool found_one_error = false;

    for (size_t i = 0; i != left_str.size(); ++i) {
      if (left_str[i] != right_str[i]) {
        if (found_one_error) return false;
        found_one_error = true;
      }
    }

    return found_one_error;
  }
};


std::vector<std::string> transpose(std::span<const std::string> lines) {
  std::vector<std::string> transposed;
  for (size_t col = 0; col < lines[0].size(); ++col) {
    transposed.emplace_back();
  }

  for (size_t row = 0; row < lines.size(); ++row) {
    for (size_t col = 0; col < lines[0].size(); ++col) {
      transposed[col] += lines[row][col];
    }
  }

  return transposed;
}

template<typename Strategy>
i64 solve(std::span<const std::string> lines) {
  Strategy is_row_symmetric;

  for (size_t row = 0; row + 1 < lines.size(); ++row) {
    if (is_row_symmetric(lines, row)) {
      return (row + 1) * 100;
    }
  }

  const auto transposed = transpose(lines);
  
  for (size_t row = 0; row + 1 < transposed.size(); ++row) {
    if (is_row_symmetric(transposed, row)) {
      return row + 1;
    }
  }

  return -1000000;
}

Output day_2023_13(const std::vector<std::string> & lines, const DayExtraInfo &) {
  i64 part_a = 0;
  i64 part_b = 0;

  size_t begin = 0;
  for (size_t i = 0; i != lines.size(); ++i) {
    if (lines[i].empty() || i + 1 == lines.size()) {
      part_a += solve<RowSymmetric     >(std::span(lines.begin() + begin, lines.begin() + i));
      part_b += solve<OffByOneSymmetric>(std::span(lines.begin() + begin, lines.begin() + i));
      begin = i + 1;
    }
  }
  
  return Output(part_a, part_b);
}
