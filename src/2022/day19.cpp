#include "../advent_of_code.hpp"
#include <array>
#include <compare>
#include <map>
#include <ranges>
#include <set>
#include <stack>
#include <regex>

// https://adventofcode.com/2022/day/19

struct EmptyStateClass {};
EmptyStateClass EmptyState;

struct Resource {
  int ore = 0;
  int clay = 0;
  int obsidian = 0;
  int geode = 0;

  Resource & operator+=(const Resource & rhs) {
    ore += rhs.ore;
    clay += rhs.clay;
    obsidian += rhs.obsidian;
    geode += rhs.geode;
    return *this;
  }
  
  Resource & operator-=(const Resource & rhs) {
    ore -= rhs.ore;
    clay -= rhs.clay;
    obsidian -= rhs.obsidian;
    geode -= rhs.geode;
    return *this;
  }

  int how_much_can_fit(const Resource & rhs) {
    int n = 999999;
    if (rhs.ore      != 0) n = std::min(n, ore / rhs.ore);
    if (rhs.clay     != 0) n = std::min(n, clay / rhs.clay);
    if (rhs.obsidian != 0) n = std::min(n, obsidian / rhs.obsidian);
    if (rhs.geode    != 0) n = std::min(n, geode / rhs.geode);
    return n;
  }

  friend std::ostream & operator<<(std::ostream & stream, const Resource & resource) {
    return stream
      << "(o=" << resource.ore
      << ", c=" << resource.clay
      << ", b=" << resource.obsidian
      << ", g=" << resource.geode
      << ")";
  }
};

struct Blueprint {
  int ore_robot_cost_ore;
  int clay_robot_cost_ore;
  int obsidian_robot_cost_ore;
  int obsidian_robot_cost_clay;
  int geode_robot_cost_ore;
  int geode_robot_cost_obsidian;
};


struct State {
  Resource current_robots;
  Resource resources;
  int time = 0;

  explicit State([[maybe_unused]] EmptyStateClass marker) {
    current_robots.ore = 1;
  }

  State do_nothing() const {
    State copy = *this;
    copy.resources += current_robots;
    copy.time += 1;
    return copy;
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

  [[nodiscard]] std::optional<State> try_buy(Resource what, const Resource & cost) const {
    if (resources.ore      < cost.ore)      return std::nullopt;
    if (resources.clay     < cost.clay)     return std::nullopt;
    if (resources.obsidian < cost.obsidian) return std::nullopt;
    if (resources.geode    < cost.geode)    return std::nullopt;

    State copy = *this;
    copy.resources -= cost;
    copy.resources += copy.current_robots;
    copy.current_robots += what;
    ++copy.time;
    return copy;
  }

  [[nodiscard]] std::optional<State> buy_ore(const Blueprint & blueprint) const {
    return try_buy(
      Resource{ 1, 0, 0, 0 },
      Resource{ blueprint.ore_robot_cost_ore }
    );
  }

  [[nodiscard]] std::optional<State> buy_clay(const Blueprint & blueprint) const {
    return try_buy(
      Resource{ 0, 1, 0, 0 },
      Resource{ blueprint.clay_robot_cost_ore }
    );
  }
  
  [[nodiscard]] std::optional<State> buy_obsidian(const Blueprint & blueprint) const {
    return try_buy(
      Resource{ 0, 0, 1, 0 },
      Resource{ blueprint.obsidian_robot_cost_ore, blueprint.obsidian_robot_cost_clay }
    );
  }
  
  [[nodiscard]] std::optional<State> buy_geode(const Blueprint & blueprint) const {
    return try_buy(
      Resource{ 0, 0, 0, 1 },
      Resource{ blueprint.geode_robot_cost_ore, 0, blueprint.geode_robot_cost_obsidian }
    );
  }

  long long int get_possible_best(const Blueprint & blueprint, int max_minutes) const {
    State cpy = *this;

    for (int i = time; i != max_minutes; ++i) {
      if (auto geode = cpy.buy_geode(blueprint)) {
        cpy = *geode;
      } else {
        ++cpy.time;
        cpy.resources += cpy.current_robots;
        ++cpy.current_robots.clay;
        ++cpy.current_robots.ore;
        ++cpy.current_robots.obsidian;
      }
      
    }

    return cpy.resources.geode;
  }

};


template<int max_minutes>
int compute_blueprint_quality(const Blueprint & blueprint) {
  std::stack<State> states;
  states.emplace(EmptyState);

  int best_quality = 0;

  while (!states.empty()) {
    const State state = states.top();
    states.pop();

    if (best_quality >= state.get_possible_best(blueprint, max_minutes)) {
      continue;
    }

    if (state.time == max_minutes) {
      best_quality = std::max(best_quality, state.resources.geode);
    } else  {
      state.add_next_states(states, blueprint);
    }
  }

  return best_quality;
}

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
        .ore_robot_cost_ore = std::stoi(smatch_[2].str()),
        .clay_robot_cost_ore = std::stoi(smatch_[3].str()),
        .obsidian_robot_cost_ore = std::stoi(smatch_[4].str()),
        .obsidian_robot_cost_clay = std::stoi(smatch_[5].str()),
        .geode_robot_cost_ore = std::stoi(smatch_[6].str()),
        .geode_robot_cost_obsidian = std::stoi(smatch_[7].str())
      }
    );
  }

  return blueprint;
}

Output day_2022_19(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Blueprint> blueprints = read_blueprints(lines);

  long long int quality_sum = 0;

  for (size_t i = 0; i != blueprints.size(); ++i) {
    const long long int my_quality = compute_blueprint_quality<24>(blueprints[i]);
    quality_sum += my_quality * (i + 1);
  }

  long long int part_b = 1;
  if (blueprints.size() > 3) {
    blueprints.erase(blueprints.begin() + 3, blueprints.end());
  }
  for (const Blueprint & blueprint : blueprints) {
    const long long int my_quality = compute_blueprint_quality<32>(blueprint);
    part_b *= my_quality;
  }

  return Output(quality_sum, part_b);
}
