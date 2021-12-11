#include "../advent_of_code.hpp"
#include <set>
#include <vector>
#include "../util/position.hpp"

// https://adventofcode.com/2021/day/11

static long long int run_one_step(std::vector<std::vector<int>> & octopuses) {
  std::set<bj::Position> already_flashed;
  std::vector<bj::Position> flash_me;

  // +1 energy
  for (int row = 0; row != 10; ++row) {
    for (int col = 0; col != 10; ++col) {
      octopuses[row][col] += 1;

      if (octopuses[row][col] > 9) {
        flash_me.emplace_back(bj::Position{ col, row });
        already_flashed.emplace(bj::Position{ col, row });
      }
    }
  }

  // Who got over 9?
  while (!flash_me.empty()) {
    const auto pos = flash_me.back();
    flash_me.pop_back();

    const auto neigh = pos.get_8_neighbours();

    for (const auto & near : neigh) {
      if (near.x < 0 || near.y < 0 || near.x >= 10 || near.y >= 10) continue;

      ++octopuses[near.y][near.x];

      if (octopuses[near.y][near.x] <= 9) continue;

      if (already_flashed.find(near) == already_flashed.end()) {
        flash_me.emplace_back(near);
        already_flashed.emplace(near);
      }
    }
  }

  // Reset
  for (const auto & flashed : already_flashed) {
    octopuses[flashed.y][flashed.x] = 0;
  }

  return already_flashed.size();
}

Output day_2021_11(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Read input
  std::vector<std::vector<int>> octopuses;

  for (const auto & line : lines) {
    octopuses.push_back({});

    for (const char c : line) {
      octopuses.back().push_back(c - '0');
    }
  }

  // Farming Simulator
  long long int nb_of_flashes = 0;
  std::optional<int> all_flash = std::nullopt;

  int current_step = 1;
  while (current_step <= 100 || !all_flash) {
    const auto flashed_now = run_one_step(octopuses);
    
    if (current_step <= 100) {
      nb_of_flashes += flashed_now;
    }

    if (flashed_now == 100 && !all_flash) {
      all_flash = current_step;
    }

    ++current_step;
  }

  return Output(nb_of_flashes, all_flash.value());
}
