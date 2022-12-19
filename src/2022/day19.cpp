#include "../advent_of_code.hpp"
#include <regex>
#include <stack>

// https://adventofcode.com/2022/day/19

namespace {

// Resources
struct Resource {
  int ore = 0;
  int clay = 0;
  int obsidian = 0;
  int geode = 0;

  constexpr Resource & operator+=(const Resource & rhs) noexcept {
    ore += rhs.ore;
    clay += rhs.clay;
    obsidian += rhs.obsidian;
    geode += rhs.geode;
    return *this;
  }
  
  friend constexpr Resource operator+(const Resource & lhs, const Resource & rhs) noexcept {
    Resource copy = lhs;
    copy += rhs;
    return copy;
  }

  constexpr Resource & operator-=(const Resource & rhs) noexcept {
    ore -= rhs.ore;
    clay -= rhs.clay;
    obsidian -= rhs.obsidian;
    geode -= rhs.geode;
    return *this;
  }

  [[nodiscard]] constexpr bool are_all_higher_or_equal(const Resource & rhs) const noexcept {
    return ore >= rhs.ore && clay >= rhs.clay && obsidian >= rhs.obsidian && geode >= rhs.geode;
  }

  friend std::ostream & operator<<(std::ostream & stream, const Resource & resource) {
    return stream
      << "[" << resource.ore << ", " << resource.clay
      << ", " << resource.obsidian << ", " << resource.geode
      << "]";
  }
};

namespace Robot {
  constexpr Resource Ore      = Resource { 1, 0, 0, 0 };
  constexpr Resource Clay     = Resource { 0, 1, 0, 0 };
  constexpr Resource Obsidian = Resource { 0, 0, 1, 0 };
  constexpr Resource Geode    = Resource { 0, 0, 0, 1 };
}

// A blueprint description
struct Blueprint {
  Resource for_ore;
  Resource for_clay;
  Resource for_obsidian;
  Resource for_geode;
};

// Marker to stop default constructing something that should be copied
struct EmptyStateClass {};
EmptyStateClass EmptyState;

// A state with the current robots, the resources and the time
struct State {
  Resource current_robots;
  Resource resources;
  int time = 0;

  // Require to bring a marker to
  // say "hey I really want to default construct this"
  explicit State([[maybe_unused]] EmptyStateClass marker) noexcept {
    current_robots.ore = 1;
  }

  friend std::ostream & operator<<(std::ostream & stream, const State & state) {
    return stream << "{ Robots= " << state.current_robots << " ; Resources= " << state.resources << " }";
  }

  void add_next_states(std::stack<State> & output, const Blueprint & blueprint) const {
    output.emplace(do_nothing());
    if (auto next = buy_ore(blueprint)) output.emplace(next.value());
    if (auto next = buy_clay(blueprint)) output.emplace(next.value());
    if (auto next = buy_obsidian(blueprint)) output.emplace(next.value());
    if (auto next = buy_geode(blueprint)) output.emplace(next.value());
  }

private:
  // Time brings all things to past
  [[nodiscard]] State do_nothing() const noexcept {
    State copy = *this;
    copy.resources += current_robots;
    copy.time += 1;
    return copy;
  }

  // Returns a new state where we bought the wanted machine, or nullopt if could not
  [[nodiscard]] std::optional<State> try_buy(const Resource & robot, const Resource & cost) const noexcept {
    if (!resources.are_all_higher_or_equal(cost)) {
      return std::nullopt;
    }

    State copy = *this;
    copy.resources -= cost;
    copy.resources += copy.current_robots;
    copy.current_robots += robot;
    ++copy.time;
    return copy;
  }

  [[nodiscard]] std::optional<State> buy_ore(const Blueprint & blueprint) const noexcept {
    return try_buy(Robot::Ore, blueprint.for_ore);
  }

  [[nodiscard]] std::optional<State> buy_clay(const Blueprint & blueprint) const noexcept {
    return try_buy(Robot::Clay, blueprint.for_clay);
  }
  
  [[nodiscard]] std::optional<State> buy_obsidian(const Blueprint & blueprint) const noexcept {
    return try_buy(Robot::Obsidian, blueprint.for_obsidian);
  }
  
  [[nodiscard]] std::optional<State> buy_geode(const Blueprint & blueprint) const noexcept {
    return try_buy(Robot::Geode, blueprint.for_geode);
  }

public:
  // Returns an evaluation of the possible best number of geodes. Fast and very approximate.
  [[nodiscard]] long long int get_possible_best(const Blueprint & blueprint, int max_minutes) const noexcept {
    State cpy = *this;

    for (int i = time; i != max_minutes; ++i) {
      if (auto geode = cpy.buy_geode(blueprint)) {
        // Can build a geode machine cool
        cpy = *geode;
      } else {
        // Exagerate an increase of capacity to be able to produce geode robots
        cpy.resources += cpy.current_robots;
        cpy.current_robots += Robot::Ore + Robot::Clay + Robot::Obsidian;
        ++cpy.time;
      }
    }

    return cpy.resources.geode;
  }
};

template<int MaxMinutes>
[[nodiscard]] int find_max_possible_geodes(const Blueprint & blueprint) {
  // DFS with state pruning
  std::stack<State> states;
  states.emplace(EmptyState);

  int max_geodes = 0;

  while (!states.empty()) {
    const State state = states.top();
    states.pop();

    if (state.time == MaxMinutes) { // End of branch
      max_geodes = std::max(max_geodes, state.resources.geode);
    } else if (max_geodes < state.get_possible_best(blueprint, MaxMinutes)) { // State pruning
      state.add_next_states(states, blueprint);
    }
  }

  return max_geodes;
}

// Transform the input into blueprints
std::vector<Blueprint> read_blueprints(const std::vector<std::string> & lines) {
  const char * regex_str = R"(^Blueprint (\d+): Each ore robot costs (\d+) ore. Each clay robot costs (\d+) ore. Each obsidian robot costs (\d+) ore and (\d+) clay. Each geode robot costs (\d+) ore and (\d+) obsidian.)";
  std::regex regex_ { regex_str };
  std::smatch smatch_;

  std::vector<Blueprint> blueprint;
  for (const std::string & line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      std::cerr << line << "\n";
      throw std::runtime_error("Bad input");
    }

    blueprint.emplace_back(
      Blueprint{
        .for_ore = Resource { .ore = std::stoi(smatch_[2].str()) },
        .for_clay = Resource { .ore = std::stoi(smatch_[3].str()) },
        .for_obsidian = Resource {
          .ore = std::stoi(smatch_[4].str()),
          .clay = std::stoi(smatch_[5].str())
        },
        .for_geode = Resource {
          .ore = std::stoi(smatch_[6].str()),
          .obsidian = std::stoi(smatch_[7].str())
        }
      }
    );
  }

  return blueprint;
}

}

Output day_2022_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Read input
  std::vector<Blueprint> blueprints = read_blueprints(lines);

  // Part A, 24 minutes, all blueprints
  long long int blueprint_quality_sum = 0;
  for (size_t i = 0; i != blueprints.size(); ++i) {
    const long long int best_found_geodes = find_max_possible_geodes<24>(blueprints[i]);
    blueprint_quality_sum += best_found_geodes * (i + 1);
  }

  // Part B, 32 minutes, first 3 blueprints
  if (blueprints.size() > 3) blueprints.erase(blueprints.begin() + 3, blueprints.end());
  long long int part_b = 1;
  for (const Blueprint & blueprint : blueprints) {
    const long long int best_found_geodes = find_max_possible_geodes<32>(blueprint);
    part_b *= best_found_geodes;
  }

  // kthxbye
  return Output(blueprint_quality_sum, part_b);
}
