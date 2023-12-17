#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <queue>
#include "../../util/position.hpp"

// #define WITH_HISTORY

using i64 = std::int64_t;
using Grid = std::vector<std::string>;

#pragma GCC diagnostic ignored "-Wsign-compare"

// https://adventofcode.com/2023/day/17

using HeatMap = std::vector<std::vector<int>>;

struct Crucible {
  explicit Crucible(bj::Direction direction) {
    position = bj::Position{ 0, 0 };
    this->direction = direction;
    direction_since = 0;
    lost_heat = 0;
  }
  
  bj::Position position;
  bj::Direction direction;
  int direction_since = 0;

  int lost_heat = 0;

#ifdef WITH_HISTORY
  std::vector<bj::Direction> history;
#endif

  struct NormalCrucible {
    static constexpr int min_step = 1;
    static constexpr int max_step = 3;
  };

  struct UltraCrucible {
    static constexpr int min_step = 4;
    static constexpr int max_step = 10;
  };

  struct Priority {
    // Most priority = less loss heat, farer on the grid
    bool operator()(const Crucible & lhs, const Crucible & rhs) const {
      return lhs.lost_heat + rhs.position.x + rhs.position.y
        > rhs.lost_heat + lhs.position.x + rhs.position.y;
    }
  };
};

class Solutions {
public:
  struct Solution {
    bj::Direction direction;
    int direction_since;
    int lost_heat;

    explicit Solution(const Crucible & crucible) {
      direction = crucible.direction;
      direction_since = crucible.direction_since;
      lost_heat = crucible.lost_heat;
    }

    [[nodiscard]] bool operator==(const Solution & other) const noexcept {
      return direction == other.direction && direction_since == other.direction_since && lost_heat == other.lost_heat;
    }

    [[nodiscard]] bool is_better_than(const Solution & other) const noexcept {
      return direction == other.direction && direction_since <= other.direction_since && lost_heat <= other.lost_heat;
    }
  };

  enum class AddResult { Equal, Accepted, Worse };

private:
  std::vector<Solution> _list;

public:
  AddResult add_solution(Solution solution) {
    for (auto it = _list.begin(); it != _list.end();) {
      const auto & existing = *it;

      if (existing.direction != solution.direction) {
        ++it;
        continue;
      }

      if (existing.is_better_than(solution)) {
        if (existing == solution) {
          return AddResult::Equal;
        } else {
          return AddResult::Worse;
        }
      } else if (solution.is_better_than(existing)) {
        // This solution is better than a previous one but we don't care that it was
        it = _list.erase(it);
      } else {
        ++it;
      }
    }

    _list.emplace_back(solution);

    return AddResult::Accepted;
  }

  [[nodiscard]] int get_minimum() const {
    if (_list.empty()) return -1;

    int min = _list[0].lost_heat;
    for (const auto & solution : _list) {
      min = std::min(min, solution.lost_heat);
    }
    return min;
  }
};

template<typename CrucibleType>
i64 solve(const HeatMap & heat_map) {
  const bj::Position target { static_cast<int>(heat_map[0].size() - 1), static_cast<int>(heat_map.size() - 1) };

  std::priority_queue<Crucible, std::vector<Crucible>, Crucible::Priority> crucible_queue;
  std::map<bj::Position, Solutions> minimum_heat;

  // Put the crucible in a way that it has to turn to the grid (ensures that part b is ok)
  for (bj::Direction direction : { bj::Direction::Top, bj::Direction::Left }) {
    Crucible crucible(direction);
    crucible_queue.push(crucible);
  }

#ifdef WITH_HISTORY
  std::multimap<int, std::vector<bj::Direction>> solutions_history;
#endif

  std::optional<int> minimum_found_heat;

  while (!crucible_queue.empty()) {
    const auto crucible = crucible_queue.top();
    crucible_queue.pop();

    // Check that this crucible is not obsolete
    if (minimum_heat[crucible.position].add_solution(Solutions::Solution(crucible)) == Solutions::AddResult::Worse) {
      continue;
    }

    // Move the crucible
    for (bj::Direction dir : { bj::Direction::Left, bj::Direction::Right, bj::Direction::Down, bj::Direction::Top }) {
      // No 180°
      if (bj::are_opposite_dirs(dir, crucible.direction)) {
        continue;
      }

      Crucible copy = crucible;

      // How many steps?
      int steps_to_do;
      if (copy.direction == dir) {
        if (copy.direction_since >= CrucibleType::max_step) {
          continue;
        }
        
        steps_to_do = 1;
        copy.direction_since += 1;
      } else {
        copy.direction = dir;
        steps_to_do = CrucibleType::min_step;
        copy.direction_since = CrucibleType::min_step;
      }

      // Move
      int number_of_steps_done = 0;
      for (; number_of_steps_done < steps_to_do; ++number_of_steps_done) {
        copy.position.move(dir);
        
#ifdef WITH_HISTORY
        copy.history.push_back(dir);
#endif

        if (copy.position.x < 0) break;
        if (copy.position.y < 0) break;
        if (copy.position.x >= heat_map[0].size()) break;
        if (copy.position.y >= heat_map.size()) break;

        copy.lost_heat += heat_map[copy.position.y][copy.position.x];
      }

      if (number_of_steps_done != steps_to_do) continue;

      // Prune solutions that are too bad
      if (minimum_found_heat) {
        // Already have a better solutions, prune it
        if (copy.lost_heat > *minimum_found_heat) {
          continue;
        }
      }

      // Register solution
      if (copy.position == target && (!minimum_found_heat || copy.lost_heat <= *minimum_found_heat)) {
        minimum_found_heat = copy.lost_heat;
        
#ifdef WITH_HISTORY
        solutions_history.emplace(copy.lost_heat, copy.history);
#endif
      }

      auto & solution = minimum_heat[copy.position];
      if (solution.add_solution(Solutions::Solution(copy)) != Solutions::AddResult::Accepted) {
        continue;
      }

      crucible_queue.push(copy);
    }
  }

  const int answer = minimum_heat[target].get_minimum();

#ifdef WITH_HISTORY
  auto solutions_range = solutions_history.equal_range(answer);
  for (auto i = solutions_range.first; i != solutions_range.second; ++i) {
    for (bj::Direction dir : i->second) {
      switch (dir) {
        case bj::Direction::Left: std::cout << '<'; break;
        case bj::Direction::Right: std::cout << '>'; break;
        case bj::Direction::Top: std::cout << '^'; break;
        case bj::Direction::Down: std::cout << 'v'; break;
      }
    }

    std::cout << '\n';
  }
#endif

  return answer;
}

HeatMap build_heat_map(const std::vector<std::string> & lines) {
  HeatMap map;
  for (const std::string & line : lines) {
    map.emplace_back();
    for (const char c : line) {
      map.back().emplace_back(c - '0');
    }
  }

  return map;
}

Output day_2023_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const HeatMap heat_map = build_heat_map(lines);
  const i64 part_a = solve<Crucible::NormalCrucible>(heat_map);
  const i64 part_b = solve<Crucible::UltraCrucible> (heat_map);
  return Output(part_a, part_b);
}
