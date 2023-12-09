#include "../../common/advent_of_code.hpp"
#include <vector>
#include <span>
#include <algorithm>
#include <set>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/9


struct Prediction {
  i64 next;
  i64 previous;

  [[nodiscard]] static Prediction predict(std::span<const i64> values) {
    Prediction retval { values.back(), values.front() };
    if (values.size() == 1) return retval;

    // Compute line below
    std::vector<i64> line_below;
    for (size_t i = 1; i < values.size(); ++i) {
      line_below.emplace_back(values[i] - values[i - 1]);
    }

    // All 0
    if (std::ranges::all_of(line_below, [](i64 v) { return v == 0; })) {
      return retval;
    }

    // Add/remove delta of line below to last/first value
    Prediction predicted = predict(line_below);

    retval.next += predicted.next;
    retval.previous -= predicted.previous;

    return retval;
  }
};


Output day_2023_09(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Prediction sum_of_predictions { 0, 0 };

  for (const std::string & line : lines) {
    std::vector<i64> numbers = bj::extract_signed_i64_from_line(line);
    const Prediction answer = Prediction::predict(numbers);
    
    sum_of_predictions.next     += answer.next;
    sum_of_predictions.previous += answer.previous;
  }

  return Output(sum_of_predictions.next, sum_of_predictions.previous);
}
