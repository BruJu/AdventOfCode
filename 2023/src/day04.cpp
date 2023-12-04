#include "../../common/advent_of_code.hpp"
#include <vector>
#include <utility>
#include <map>
#include <cstring>
#include <set>

// https://adventofcode.com/2023/day/4

static std::set<int> to_set(std::span<const int> values) {
  return std::set<int>(values.begin(), values.end());
}

Output day_2023_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::map<int, long long int> number_of_owned_scratched_cards;
  std::set<int> existing_cards;

  int part_a = 0;

  for (const std::string & line : lines) {
    const size_t colon_pos = line.find(":");

    const int game_number = std::stoi(line.substr(std::strlen("Game "), colon_pos - std::strlen("Game ")));

    // [B] Get the free scratch card
    existing_cards.emplace(game_number);
    ++number_of_owned_scratched_cards[game_number];

    // Read common numbers
    const auto split = bj::string_split(line.substr(colon_pos + 1), "|");

    const auto winning_numbers = to_set(bj::extract_ints_from_line(split[0]));
    const auto my_numbers = to_set(bj::extract_ints_from_line(split[1]));

    std::vector<int> shared;
    std::set_intersection(
      winning_numbers.begin(), winning_numbers.end(),
      my_numbers.begin(), my_numbers.end(),
      std::back_inserter(shared)
    );

    if (shared.size() > 0) {
      // [A] Add 2^(shared numbers - 1)
      part_a += 1 << (shared.size() - 1);

      // [B] Give the (shared numbers - 1) subsequent scratch cards
      for (size_t i = 0; i < shared.size(); ++i) {
        number_of_owned_scratched_cards[game_number + i + 1] += number_of_owned_scratched_cards[game_number];
      }
    }
  }

  // [B] Count number of owned scratch cards
  long long int part_b = 0;
  for (const auto scratch_card_id : existing_cards) {
    part_b += number_of_owned_scratched_cards[scratch_card_id];
  }

  return Output(part_a, part_b);
}
