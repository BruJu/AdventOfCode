#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <algorithm>
#include <vector>
#include <cstring>

// https://adventofcode.com/2017/day/24

// It's probably possible to refactor this using the pattern strategy
// and templates

int build_strong_bridge(
  int exposed_pins,
  std::vector<std::pair<int, int>> & available_components,
  std::vector<bool> & picked_components
) {
  int best = 0;

  for (size_t i = 0; i != available_components.size(); ++i) {
    if (picked_components[i]) continue;

    auto a = available_components[i].first;
    auto b = available_components[i].second;

    picked_components[i] = true;

    if (a == exposed_pins) {
      auto my_best = build_strong_bridge(b, available_components, picked_components)
        + a + b;

      best = std::max(my_best, best);
    } else if (b == exposed_pins) {
      auto my_best = build_strong_bridge(a, available_components, picked_components)
        + a + b;

      best = std::max(my_best, best);
    }

    picked_components[i] = false;
  }

  return best;
}

struct LongBridge {
  size_t length = 0;
  int strength = 0;

  bool operator<(const LongBridge & other) const {
    if (length < other.length) return true;
    if (length > other.length) return false;
    return strength < other.strength;
  }
};

LongBridge build_long_bridge(
  int exposed_pins,
  std::vector<std::pair<int, int>> & available_components,
  std::vector<bool> & picked_components,
  size_t size = 0
) {
  LongBridge best { .length = 0, .strength = 0 };

  for (size_t i = 0; i != available_components.size(); ++i) {
    if (picked_components[i]) continue;

    auto a = available_components[i].first;
    auto b = available_components[i].second;

    picked_components[i] = true;

    LongBridge my_best;
    if (a == exposed_pins) {
      my_best = build_long_bridge(b, available_components, picked_components, size + 1);
      my_best.length += 1;
      my_best.strength += a + b;
    } else if (b == exposed_pins) {
      my_best = build_long_bridge(a, available_components, picked_components, size + 1);
      my_best.length += 1;
      my_best.strength += a + b;
    }

    if (best < my_best) {
      best = my_best;
    }

    picked_components[i] = false;
  }

  return best;
}


Output day_2017_24(const std::vector<std::string> & lines, const DayExtraInfo &) {

  std::vector<std::pair<int, int>> components;

  for (const std::string & line : lines) {
    const auto bridge = bj::string_split(line, "/");
    components.emplace_back(std::make_pair(std::stoi(bridge[0]), std::stoi(bridge[1])));
  }

  std::vector<bool> picked_components;
  picked_components.resize(components.size(), false);

  int best_bridge = build_strong_bridge(0, components, picked_components);
  LongBridge longest_bridge = build_long_bridge(0, components, picked_components);

  return Output(best_bridge, longest_bridge.strength);
}