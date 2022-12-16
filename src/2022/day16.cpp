#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <set>
#include <list>
#include <array>
#include <optional>
#include <bitset>
#include <array>
#include <span>
// https://adventofcode.com/2022/day/16


using CompactName = size_t;
constexpr size_t MaxNonEmptyValves = 16;

template<typename T, size_t N>
struct bad_static_vector {
private:
  std::array<T, N> values {};
  size_t size = 0;

public:
  void add(T value) {
    if (size == N) throw std::runtime_error("too much elemensts in bad static vector");
    values[size] = value;
    ++size;
  }

  [[nodiscard]] std::span<const T> get_all() const {
    return std::span(values.begin(), values.begin() + size);
  }
};


struct Valve {
  int flow_rate;
  bad_static_vector<CompactName, 6> lead_to;
};

struct Valves {
private:
  std::map<std::string, CompactName> name_to_value;
  std::vector<Valve> m_valves;
  
  size_t valves_to_open;
  unsigned long open_valves_mask;

public:
  explicit Valves(const std::vector<std::string> & lines);

  [[nodiscard]] const Valve & operator[](const CompactName name) const {
    return m_valves[name];
  }

  [[nodiscard]] size_t number_of_valves_to_open() const noexcept {
    return valves_to_open;
  }

  [[nodiscard]] CompactName get_id_of(const std::string & name) const {
    return name_to_value.find(name)->second;
  }

  [[nodiscard]] unsigned long get_open_valve_mask() const noexcept {
    return open_valves_mask;
  }
};

Valves::Valves(const std::vector<std::string> & lines) {
  std::regex regex_ { R"(Valve ([A-Z][A-Z]) has flow rate=(\d+); tunnels? leads? to valves? (.*))"};
  std::smatch smatch_;

  m_valves.reserve(lines.size());

  const auto create_or_get_valve = [&](const std::string & name) {
    if (auto it = name_to_value.find(name); it != name_to_value.end()) {
      return &m_valves[it->second];
    } else {
      name_to_value.emplace(name, m_valves.size());
      return &m_valves.emplace_back();
    }
  };

  // Put non empty flow_rate first
  for (const auto & line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      throw std::runtime_error(":(");
    }

    const int flow_rate = std::stoi(smatch_[2].str());
    if (flow_rate != 0) {
      const std::string str_name = smatch_[1].str();
      create_or_get_valve(str_name);
    }
  }

  // Ok, read tunnels and put empty tunnels
  for (const auto & line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      throw std::runtime_error(":(");
    }

    const std::string str_name = smatch_[1].str();
    Valve * ptrValve = create_or_get_valve(str_name);

    ptrValve->flow_rate = std::stoi(smatch_[2].str());
    std::vector<std::string> next_name = bj::string_split(smatch_[3].str(), ", ");
    for (const auto & name : next_name) {
      create_or_get_valve(name);
      ptrValve->lead_to.add(get_id_of(name));
    }
  }

  // Cache the number of valves to open
  valves_to_open = 0;
  for (const Valve & valve : m_valves) {
    if (valve.flow_rate != 0) {
      ++valves_to_open;
    }
  }

  if (valves_to_open >= MaxNonEmptyValves) {
    throw std::runtime_error("Too much valves to open");
  }

  open_valves_mask = (1 << valves_to_open) - 1;
}

struct State {
  std::bitset<MaxNonEmptyValves> oppened;
  long int total_pressure = 0;

  CompactName on_name;

  void next_states(
    std::vector<State> & output,
    const Valves & valves, int remaining) const {

    const Valve & valve = valves[on_name];

    if (valve.flow_rate != 0 && !oppened.test(on_name)) {
      State copy = *this;
      copy.oppened.set(on_name);
      if (remaining >= 0)
        copy.total_pressure += remaining * valve.flow_rate;
      output.emplace_back(copy);
    }

    for (const CompactName & next_valve : valve.lead_to.get_all()) {
      State copy = *this;
      copy.on_name = next_valve;
      output.emplace_back(copy);
    }
  }

};


bool order_states(const State & lhs, const State & rhs, const unsigned long all_open_mask) {
  const auto lhs_op = lhs.oppened.to_ulong();
  const auto rhs_op = rhs.oppened.to_ulong();
  if (lhs_op < rhs_op) return false;
  if (lhs_op > rhs_op) return true;

  if (lhs_op != all_open_mask) {
    if (lhs.on_name < rhs.on_name) return true;
    if (lhs.on_name > rhs.on_name) return false;
  }

  if (lhs.total_pressure > rhs.total_pressure) return true;
  if (lhs.total_pressure < rhs.total_pressure) return false;

  return false;
}

bool rhs_is_useless(const State & lhs, const State & rhs, const unsigned long all_open_mask) {
  return lhs.oppened == rhs.oppened 
    && (lhs.oppened.to_ulong() == all_open_mask || lhs.on_name == rhs.on_name);
}


Output day_2022_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Valves valves(lines);
  const auto all_open_mask = valves.get_open_valve_mask();  

  std::vector<State> current_states;
  {
    State state;
    state.on_name = valves.get_id_of("AA");
    current_states.emplace_back(state);
  }


  std::vector<State> next;
  for (int i = 0; i != 30; ++i) {
    std::sort(current_states.begin(), current_states.end(),
      [&](const State & lhs, const State & rhs) {
        return order_states(lhs, rhs, all_open_mask);
      }
    );

    const auto end = std::unique(
      current_states.begin(),
      current_states.end(),
      [&](const State & lhs, const State & rhs) {
        return rhs_is_useless(lhs, rhs, all_open_mask);
      }
    );

    current_states.erase(end, current_states.end());

    std::cout << i << " " << current_states.size() << "\n";
    
    next.clear();
    for (const State & s : current_states) {
      if (s.oppened.to_ulong() == all_open_mask) {
        next.emplace_back(s);
        continue;
      }

      s.next_states(next, valves, 30 - i - 1);
    }

    if (next.size() == 0) {
      next.emplace_back(current_states[0]);
    }

    std::swap(current_states, next);
  }

  const auto max_pressure = std::max_element(
    current_states.begin(), current_states.end(),
    [](const State & state, const State & rhs) {
      return state.total_pressure < rhs.total_pressure;
    }
  );

  return Output(max_pressure->total_pressure, 0);
}
