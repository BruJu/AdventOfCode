#include "../advent_of_code.hpp"
#include <map>
#include <exception>

// https://adventofcode.com/2016/day/20#part2
// List of blocked values
// (A) Find the smallest allowed
// (B) Count the number of allowed

Output day_2016_20(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::map<std::uint32_t, std::uint32_t> intervals;

  for (const std::string & line : lines) {
    const auto separator = line.find('-');

    const std::uint32_t left  = std::stoul(line.substr(0, separator));
    const std::uint32_t right = std::stoul(line.substr(separator + 1));

    const bool b = intervals.try_emplace(left, right).second;
    if (!b) {
      throw std::runtime_error("multiple intervals start with a value");
    }
  }

  // Part A
  std::uint32_t lowest_value = 0;
  for (const auto & [min, max] : intervals) {
    if (min > lowest_value) break;
    lowest_value = std::max(max + 1, lowest_value);
  }

  // Part B
  std::uint32_t amount_of_allowed = 0;
  
  {
    std::uint32_t exit_at = dei.part_b_extra_param != 0 ? dei.part_b_extra_param : 0xFFFFFFFF;
    std::uint32_t maybe_next = 0;

    bool finished = false;

    for (const auto & [min, max] : intervals) {
      if (maybe_next < min) {
        amount_of_allowed += (min - maybe_next);
        maybe_next = min;
      }

      if (max == exit_at) {
        finished = true;
        break;
      }

      maybe_next = std::max(max + 1, maybe_next);
    }

    if (!finished) {
      // Julian 🦆 — Aujourd’hui à 11:03
      // réalisation du jour :
      // on ne peut pas compter le nombre de uint32 que l'on peut stocker dans un uint32 dans un uint32
      // vu qu'il y a 2^32 valeurs différentes, mais seules les valeurs entre 0 et 2^32 - 1 sont autorisées

      amount_of_allowed += (exit_at - maybe_next + 1);
    }
  }

  return Output(lowest_value, amount_of_allowed);
}

