#include "../../common/advent_of_code.hpp"
#include <map>
#include <set>
#include <span>
#include <algorithm>
#include <ranges>
#include <utility>

// https://adventofcode.com/2024/day/5

/**
 * Add the rank of each value in the order described by rank_map
 */
static
std::vector<std::pair<int, int>> with_ranks(std::span<const int> values, const std::map<int, int> & rank_map) {
  return values
  | std::views::transform(
    [&](const int value) -> std::pair<int, int> { return *rank_map.find(value); }
  )
  | std::ranges::to< std::vector<std::pair<int, int>> >();
}

/**
 * Filter from the list of pairs the pairs that only contains values in `accepted_values`.
 */
static
std::vector<std::pair<int, int>> filter_out_missing(
  std::vector<std::pair<int, int>> pairs,
  std::span<const int> accepted_values
) {
  const std::set<int> accepted_values_set = accepted_values | std::ranges::to<std::set<int>>();

  return pairs
    | std::views::filter(
      [&](const auto & comp) {
        return accepted_values_set.contains(comp.first) && accepted_values_set.contains(comp.second);
      }
    )
    | std::ranges::to< std::vector<std::pair<int, int>> >();
}

/**
 * Compute the rank map = the mapping value -> its rank described by the known_comparisons ordering.
 * Comparisons that contains values out of the `accepted_values` list are ignored.
 */
static
std::map<int, int> compute_rank_map(
  std::vector<std::pair<int, int>> known_comparisons,
  std::span<const int> accepted_values
) {
  known_comparisons = filter_out_missing(known_comparisons, accepted_values);

  // Build the returned value
  std::map<int, int> rank_map;

  // Init all levels to 1 or 2 (note: this could be merged with the second loop)
  for (const auto & [a, b] : known_comparisons) {
    rank_map[a] = std::max(rank_map[a], 1);
    rank_map[b] = rank_map[a] + 1;
  }

  // Build the ordering
  while (true) {
    bool stable = true;

    for (const auto & [a, b] : known_comparisons) {
      const int a_rank = rank_map[a];
      int & b_rank = rank_map[b];

      if (a_rank >= b_rank) {
        // b should be greater than a in the order -> it should be at least a's level + 1
        b_rank = a_rank + 1;
        stable = false;
      }

      if (std::cmp_greater(a_rank, known_comparisons.size() + 5)) {
        throw std::runtime_error("2024-05 cycle detected");
      }
    }

    if (stable) break;
  }

  return rank_map;
}

struct PuzzleInput {
  std::vector<std::pair<int, int>> known_comp;
  std::vector<std::vector<int>> page_lists;
};

static
PuzzleInput read_puzzle_input(const std::vector<std::string> & lines) {
  PuzzleInput retval;

  for (const std::string & line : lines) {
    // Relying on the fact that the two parts are formatted differently
    // and not using the fact that there is an empty line
    // between them.
    if (line.empty()) continue;

    const size_t comp_separator = line.find('|');
    if (comp_separator != std::string::npos) {
      const int before = std::stoi(line.substr(0, comp_separator));
      const int after = std::stoi(line.substr(comp_separator + 1));
      retval.known_comp.emplace_back(before, after);
    } else {
      retval.page_lists.emplace_back(
        StringSplitter::line_to_vector<int>(
          line,
          [](const std::string & s) { return std::stoi(s); },
          ','
        )
      );
    }
  }

  return retval;
}


Output day_2024_05(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto & [known_comp, page_lists] = read_puzzle_input(lines);

  long long int part_a = 0; // Middle value of well ordered pages
  long long int part_b = 0; // Middle value of reordered non ordered pages

  for (const auto & page_list : page_lists) {
    // Build order for this page.
    // Can't do it for the whold known comp because of cycles.
    const std::map<int, int> number_ranks = compute_rank_map(known_comp, page_list);

    // Add ranks to page
    std::vector<std::pair<int, int>> page_with_ranks = with_ranks(page_list, number_ranks);

    struct RankCompare {
      [[nodiscard]] bool operator()(const std::pair<int, int> & lhs, const std::pair<int, int> & rhs) const noexcept {
        return lhs.second < rhs.second;
      }
    };

    if (std::is_sorted(page_with_ranks.begin(), page_with_ranks.end(), RankCompare{})) {
      // Ok for part A
      part_a += page_with_ranks[page_with_ranks.size() / 2].first; // Value of middle number
    } else {
      // Sort for part B
      // std::sort(page_with_ranks.begin(), page_with_ranks.end(), RankCompare{});
      
      // I lied, just going to find the answer
      std::nth_element(
        page_with_ranks.begin(),
        page_with_ranks.begin() + (page_with_ranks.size() / 2),
        page_with_ranks.end(),
        RankCompare{}
      );

      part_b += page_with_ranks[page_with_ranks.size() / 2].first; // Value of middle number when properly sorted
    }
  }

  return Output(part_a, part_b);
}
