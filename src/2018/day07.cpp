#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <bitset>
#include <set>
#include <string>
#include <span>
#include <map>
#include <ranges>

// https://adventofcode.com/2018/day/07



Output day_2018_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::set<char> all_steps;
  std::vector<std::pair<char, char>> requirements;

  for (const std::string & line : lines) {
    // Step C must be finished before step A can begin.
    // 0123456789012345678901234567890123456789
    //      ^ 5                            ^ 36

    requirements.emplace_back(std::pair<char, char>(line[5], line[36]));
    all_steps.emplace(line[5]);
    all_steps.emplace(line[36]);
  }

  std::string ordered_steps;
  std::set<char> already_placed;
  std::set<char> available;
  std::map<char, std::vector<char>> prev_to_next;
  std::map<char, std::vector<char>> next_to_prev;

  for (const auto & [required, next_task] : requirements) {
    prev_to_next[required].emplace_back(next_task);
    next_to_prev[next_task].emplace_back(required);
  }

  for (const char step : all_steps) {
    if (!next_to_prev.contains(step)) {
      available.emplace(step);
    }
  }

  while (!available.empty()) {
    // Get next task
    const char do_now = *available.begin();
    available.erase(available.begin());

    // Do it
    ordered_steps += do_now;
    already_placed.emplace(do_now);

    // What can we do now?
    const auto nexts = prev_to_next.find(do_now);
    if (nexts == prev_to_next.end()) continue;

    for (const char possible_next : nexts->second) {
      const auto & previouses = next_to_prev.find(possible_next)->second;

      const bool all_previous_done = std::all_of(
        previouses.begin(), previouses.end(),
        [&](const char required) {
          return already_placed.contains(required);
        }
      );

      if (all_previous_done) {
        available.emplace(possible_next);
      }
    }
  }


  




  return Output(ordered_steps, 0);
}