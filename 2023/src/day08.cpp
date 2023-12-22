#include "../../common/advent_of_code.hpp"
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <set>
#include "../../util/prime_decomposition.hpp"

using i64 = long long int;

// https://adventofcode.com/2023/day/8

i64 compute_lcm(std::span<const i64> values) {
  std::map<i64, i64> factors_to_power;
  Dividers dividers;

  for (const i64 value : values) {
    for (const auto divider : dividers.get_prime_decomposition(value)) {
      factors_to_power[divider.value] = std::max<i64>(divider.power, factors_to_power[divider.value]);
    }
  }

  i64 lcm = 1;
  for (const auto & [factor, power] : factors_to_power) {
    for (i64 i = 0; i != power; ++i) {
      lcm *= factor;
    }
  }

  return lcm;
}

class Graph {
private:
  std::string m_directions;
  std::map<int, std::pair<int, int>> m_paths;

  std::set<i64> m_start; // Part B
  std::set<i64> m_end;   // Part B

public:
  Graph(const std::string & directions, std::span<const std::string> paths) {
    m_directions = directions;

    for (const std::string & path : paths) {
      add_path(path);
    }
  }

private:
  void add_path(const std::string & str) {
    const i64 from = add_node(str.substr(0, 3));
    const i64 left = add_node(str.substr(str.find("(") + 1, 3));
    const i64 right = add_node(str.substr(str.find(", ") + 2, 3));

    m_paths[from] = std::pair(left, right);
  }

  i64 add_node(const std::string & s) {
    const i64 code = get_node(s);
    if (s[2] == 'A') m_start.emplace(code);
    if (s[2] == 'Z') m_end.emplace(code);
    return code;
  }

public:
  static i64 get_node(const std::string & s) {
    return s[0] * 0x10000 + s[1] * 0x100 + s[2];
  }

  struct Iterator {
    i64 current_node;
    Graph * graph;
    i64 step = 0;

    Iterator(i64 start_node, Graph * graph)
    : current_node(start_node), graph(graph) {}

    Iterator & operator++() {
      char dir = graph->m_directions[step % graph->m_directions.size()];
      ++step;

      if (dir == 'L') {
        current_node = graph->m_paths[current_node].first;
      } else {
        current_node = graph->m_paths[current_node].second;
      }
      
      return *this;
    }

    [[nodiscard]] bool operator!=(i64 node) const noexcept {
      return current_node != node;
    }

    [[nodiscard]] bool operator!=(const std::set<i64> & nodes) const {
      return !nodes.contains(current_node);
    }
  };

  i64 solve_part_a() {
    Iterator it { Graph::get_node("AAA"), this };
    const i64 target = Graph::get_node("ZZZ");

    while (it != target) {
      ++it;
    }

    return it.step;
  }

  i64 solve_part_b() {
    std::vector<i64> cycles;

    // Find cycles
    for (const i64 start_node : m_start) {
      Iterator it { start_node, this };

      // Find end
      while (it != m_end) {
        ++it;
      }

      const i64 first_cycle = it.step;

      // Reached the end in n steps. Try to reach end again in n * 2 steps
      ++it;

      while (it != m_end) {
        ++it;
      }

      // Does lcm approach work?
      if (first_cycle * 2 != it.step) {
        std::cerr << first_cycle << "\n";
        std::cerr << it.step << "\n";
        throw std::runtime_error("Solution only works if end is reached every X steps");
      }

      // Yep
      cycles.emplace_back(first_cycle);
    }

    // Compute LCM
    return compute_lcm(cycles);
  }
};

Output day_2023_08(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  Graph g { lines[0], std::span(lines.begin() + 2, lines.end()) };
  
  const i64 part_a = dei.can_skip_part_A ? 0 : g.solve_part_a();
  const i64 part_b = dei.can_skip_part_B ? 0 : g.solve_part_b();

  return Output(part_a, part_b);
}
