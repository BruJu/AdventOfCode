#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <set>
#include <list>
// https://adventofcode.com/2022/day/16


using CompactName = int;
int compact_name(const std::string & s) { return s[0] * 0x100 + s[1]; }

struct Valve {
  CompactName name;
  int flow_rate;
  std::vector<CompactName> lead_to;
};

using Valves = std::map<CompactName, Valve>;

const Valve & find_valve(const Valves & valves, CompactName name) {
  return valves.find(name)->second;
}

struct State {
  std::set<CompactName> oppened;
  CompactName on_name = compact_name("AA");
  long long int total_pressure = 0;
  int minute = 0;

  std::vector<State> next_states(const Valves & valves, int remaining) const {
    std::vector<State> next;

    const Valve & valve = find_valve(valves, on_name);

    if (valve.flow_rate != 0 && !oppened.contains(on_name)) {
      State copy = *this;
      copy.oppened.emplace(on_name);
      if (remaining >= 0)
        copy.total_pressure += remaining * valve.flow_rate;
      next.emplace_back(copy);
    }

    for (const CompactName & next_valve : valve.lead_to) {
      State copy = *this;
      copy.on_name = next_valve;
      next.emplace_back(copy);
    }

    return next;
  }

};



Output day_2022_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::regex regex_ { R"(Valve ([A-Z][A-Z]) has flow rate=(\d+); tunnels? leads? to valves? (.*))"};
  std::smatch smatch_;

  std::map<CompactName, Valve> valves;

  for (const auto line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      throw std::runtime_error(":(");
    }

    Valve valve;
    valve.name = compact_name(smatch_[1].str());
    valve.flow_rate = std::stoi(smatch_[2].str());
    std::vector<std::string> next_name = bj::string_split(smatch_[3].str(), ", ");
    for (const auto & name : next_name) {
      valve.lead_to.emplace_back(compact_name(name));
    }

    valves.emplace(valve.name, valve);
  }

  State state;
  state.total_pressure = 0;
  state.on_name = compact_name("AA");

  int x = 0;

  std::vector<State> current_states;
  current_states.emplace_back(state);

  int number_to_open = 0;
  for (const Valve & valve : valves | std::views::values) {
    if (valve.flow_rate != 0) ++number_to_open;
  }

  for (int i = 0; i != 30; ++i) {

    std::sort(
      current_states.begin(), current_states.end(),
      [&](const State & lhs, const State & rhs) {
        if (lhs.oppened.size() != number_to_open) {
          if (lhs.on_name < rhs.on_name) return true;
          if (lhs.on_name > rhs.on_name) return false;
        }
        if (lhs.oppened.size() < rhs.oppened.size()) return true;
        if (lhs.oppened.size() > rhs.oppened.size()) return false;

        auto lit = lhs.oppened.begin();
        auto rit = rhs.oppened.begin();
        while (lit != lhs.oppened.end()) {
          if (*lit < *rit) return true;
          if (*lit > *rit) return false;
          ++lit;
          ++rit;
        }

        if (lhs.total_pressure > rhs.total_pressure) return true;
        if (lhs.total_pressure < rhs.total_pressure) return false;

        return false;
      }
    );

    std::list<State> listed(current_states.begin(), current_states.end());

    auto listed_it = listed.begin();
    auto listed_next = listed.begin();
    ++listed_next;
    while (listed_it != listed.end() && listed_next != listed.end()) {
      const auto & lhs = *listed_it;
      const auto & rhs = *listed_next;
      if (
        ( lhs.oppened.size() == rhs.oppened.size() && lhs.oppened.size() == number_to_open )
        || ( lhs.on_name == rhs.on_name && lhs.oppened == rhs.oppened)
        ) {
          listed_next = listed.erase(listed_next);
      } else {
        ++listed_it;
        ++listed_next;
      }
    }

    current_states = std::vector<State>(listed.begin(), listed.end());

    std::cout << i << " " << current_states.size() << "\n";
    std::vector<State> next;

    for (const State & s : current_states) {
      if (state.oppened.size() == number_to_open) {
        next.emplace_back(s);
        continue;
      }

      const auto nexts = s.next_states(valves, 30 - i - 1);

      for (const auto & n : nexts) {
        next.emplace_back(n);
      }
    }

    if (next.size() == 0) {
      next = current_states;
    }

    current_states = next;
  }

  std::partial_sort(current_states.begin(), current_states.begin() + 1, current_states.end(),
  [](const State & state, const State & rhs) {
    return state.total_pressure > rhs.total_pressure;
  }
  );



  return Output(current_states[0].total_pressure, 0);
}
