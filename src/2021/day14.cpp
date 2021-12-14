#include "../advent_of_code.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <array>
#include "../util/position.hpp"

// https://adventofcode.com/2021/day/14

///////////////////////////////////////////////////////////////////////////////

using Sequence = std::pair<char, char>;
using FlatSequence = std::map<Sequence, long long int>;
using Transformations = std::map<Sequence, std::array<Sequence, 2>>;

static FlatSequence next_step(const FlatSequence & current, const Transformations & transformations) {
  FlatSequence next;

  for (const auto & [sequence, times] : current) {
    const auto [a, b] = transformations.find(sequence)->second;

    next[a] += times;
    next[b] += times;
  }

  return next;
}

static std::map<char, long long int> count_occurrences(const FlatSequence & flat_seq) {
  std::map<char, long long int> count;

  for (const auto & [sequence, times] : flat_seq) {
    count[sequence.first] += times;
    count[sequence.second] += times;
  }

  return count;
}

long long int answer(const FlatSequence & flat_seq) {
  const auto count = count_occurrences(flat_seq);

  std::optional<long long int> min;
  std::optional<long long int> max;

  for (const auto & [_, nb_occ] : count) {
    if (!min || nb_occ < *min) min = nb_occ;
    if (!max || nb_occ > *max) max = nb_occ;
  }

  return *max - *min;
}

long long int length(const FlatSequence & flat_seq) {
  long long int c = 1;

  for (const auto & [_, cnt] : flat_seq) {
    c += cnt;
  }

  return c;
}


Output day_2021_14(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  FlatSequence current;

  std::string template_ = lines[0];
  for (size_t i = 0; i != template_.size() - 1; ++i) {
    current[Sequence{ template_[i], template_[i + 1] }] += 1;
  }

  Transformations transformations;
  for (auto it = lines.begin() + 2; it != lines.end(); ++it) {
    const auto & this_trans = *it;
    transformations[Sequence{ this_trans[0], this_trans[1] }] = {
      Sequence{ this_trans[0], this_trans[6] },
      Sequence{ this_trans[6], this_trans[1] }
    };
  }
    std::cout << answer(current) << ' ' << length(current) << '\n';

  for (int step = 0; step != 10; ++step) {
    current = next_step(current, transformations);
    std::cout << answer(current) << " length=" << length(current) <<'\n';
  }

  // Count most frequent - less frequent

  long long int part_a = answer(current);

  return Output(part_a, 0);
}
