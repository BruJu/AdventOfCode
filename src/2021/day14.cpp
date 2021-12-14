#include "../advent_of_code.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <array>
#include "../util/position.hpp"

// https://adventofcode.com/2021/day/14


/** A letter and the next one */
using Domino = std::pair<char, char>;

/** Number of occurrences of a domino */
using Dominos = std::map<Domino, long long int>;

/** The two Domino to create for each Domino */
using Transformations = std::map<Domino, std::array<Domino, 2>>;

/** Process a transformation */
static Dominos next_step(const Dominos & current, const Transformations & transformations) {
  Dominos next;

  for (const auto & [domino, times] : current) {
    const auto [a, b] = transformations.find(domino)->second;

    next[a] += times;
    next[b] += times;
  }

  return next;
}

/** Return the differrence between the max nb of occurrences of a letter and the min */
static long long int answer(char first_letter, const Dominos & flat_seq) {
  // == Count number of occurrences of each letter.
  // First letter -> Not on the right side of any domino
  std::map<char, long long int> count { { first_letter, 1 } };
  // Count right side of all dominos
  for (const auto & [sequence, times] : flat_seq) {
    count[sequence.second] += times;
  }

  // == Look for min and max
  std::optional<long long int> min;
  std::optional<long long int> max;

  for (const auto & [_, nb_occ] : count) {
    if (!min || nb_occ < *min) min = nb_occ;
    if (!max || nb_occ > *max) max = nb_occ;
  }

  return *max - *min;
}

Output day_2021_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // == Transform the input into a list of dominos
  Dominos current;

  std::string template_ = lines[0];
  for (size_t i = 0; i != template_.size() - 1; ++i) {
    current[Domino{ template_[i], template_[i + 1] }] += 1;
  }

  // == Read the transformations
  Transformations transformations;
  for (auto it = lines.begin() + 2; it != lines.end(); ++it) {
    const auto & this_trans = *it;
    transformations[Domino{ this_trans[0], this_trans[1] }] = {
      Domino{ this_trans[0], this_trans[6] },
      Domino{ this_trans[6], this_trans[1] }
    };
  }

  // == Part A
  for (int step = 0; step != 10; ++step) {
    current = next_step(current, transformations);
  }
  const long long int part_a = answer(template_[0], current);

  // == Part B
  for (int step = 10; step != 40; ++step) {
    current = next_step(current, transformations);
  }
  const long long int part_b = answer(template_[0], current);

  // Ok
  return Output(part_a, part_b);
}
