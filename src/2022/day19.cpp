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

  explicit State([[maybe_unused]] EmptyStateClass marker) {
    current_robots.ore = 1;
  }

  friend std::ostream & operator<<(std::ostream & stream, const State & state) {
    return stream << "{ Robots= " << state.current_robots << " ; Resources= " << state.resources << " }";
  }

  std::vector<State> nextStates(const Blueprint & blueprint) const {
    std::vector<State> nexts { *this };

    nexts = generate_next(std::move(nexts), [&](const State & state) { return state.buy_geode(blueprint); });
    nexts = generate_next(std::move(nexts), [&](const State & state) { return state.buy_obsidian(blueprint); });
    nexts = generate_next(std::move(nexts), [&](const State & state) { return state.buy_clay(blueprint); });
    nexts = generate_next(std::move(nexts), [&](const State & state) { return state.buy_ore(blueprint); });


    for (State & state : nexts) {
      state.resources += current_robots;
    }

    return nexts;
  }

  template<typename Generator>
  static std::vector<State> generate_next(std::vector<State> from, Generator generator) {
    std::vector<State> to;

    for (const State & current : from) {
      to.emplace_back(current);

      State c = current;
      while (auto next_c = generator(c)) {
        to.emplace_back(next_c.value());
        c = next_c.value();
      }
    }

    return to;
  }

  [[nodiscard]] std::optional<State> try_buy(Resource what, const Resource & cost) const {
    if (resources.ore      < cost.ore)      return std::nullopt;
    if (resources.clay     < cost.clay)     return std::nullopt;
    if (resources.obsidian < cost.obsidian) return std::nullopt;
    if (resources.geode    < cost.geode)    return std::nullopt;

    State copy = *this;
    copy.resources -= cost;
    copy.current_robots += what;
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

  long long int end_simulation(int remaining_minutes) const {
    return resources.geode + remaining_minutes * current_robots.geode;
  }

  static long long int get_astar_score(const Blueprint & blueprint, State state, int remaining_minutes) {
    while (remaining_minutes > 0) {
      State final_state = state;

      final_state.current_robots.obsidian += final_state.resources.how_much_can_fit(Resource{ blueprint.geode_robot_cost_ore, 0, blueprint.geode_robot_cost_obsidian });
      final_state.current_robots.obsidian += final_state.resources.how_much_can_fit(Resource{ blueprint.obsidian_robot_cost_ore, blueprint.obsidian_robot_cost_clay });
      final_state.current_robots.obsidian += final_state.resources.how_much_can_fit(Resource{ blueprint.clay_robot_cost_ore });
      final_state.current_robots.obsidian += final_state.resources.how_much_can_fit(Resource{ blueprint.ore_robot_cost_ore });

      final_state.resources += state.current_robots;

      --remaining_minutes;

      state = final_state;
    }

    return state.resources.geode;
  }




};


int compute_blueprint_quality(const Blueprint & blueprint) {
  std::vector<State> states;
  states.emplace_back(EmptyState);

  static constexpr int end_minute = 24;
  for (int minute = 0; minute != end_minute; ++minute) {
    std::cout << "minute=" << minute << " " << states.size() << "\n";
    std::vector<State> next;
    for (const State & state : states) {
      const auto res = state.nextStates(blueprint);
      for (const State & state : res) {
        next.emplace_back(state);
      }
    }
    states = next;

    const int left_minutes = end_minute - minute - 1;
    const auto best = std::max_element(states.begin(), states.end(),
      [&](const State & lhs, const State & rhs) {
        return lhs.end_simulation(left_minutes) < rhs.end_simulation(left_minutes);
      }
    )->end_simulation(left_minutes);

    std::erase_if(states,
      [&](const State & state) {
        return State::get_astar_score(blueprint, state, left_minutes) < best;
      }
    );

  }

  const auto best = std::max_element(
    states.begin(), states.end(),
    [](const State & lhs, const State & rhs) {
      return lhs.resources.geode < rhs.resources.geode;
    }
  );
  return best->resources.geode;
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
  const std::vector<Blueprint> blueprints = read_blueprints(lines);

  long long int quality_sum = 0;
  for (size_t i = 0; i != blueprints.size(); ++i) {
    const long long int my_quality = compute_blueprint_quality(blueprints[i]);
    std::cout << (i + 1) << " = " << my_quality << "\n";
    quality_sum += my_quality * (i + 1);
  }
  

  return Output(quality_sum, 0);
}
