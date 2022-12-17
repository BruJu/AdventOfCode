#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>
#include <chrono>
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

namespace {

using CompactName = size_t;
constexpr size_t MaxNonEmptyValves = 16;
constexpr size_t MaxValves = 64;

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
  
  [[nodiscard]] std::span<T> get_all() {
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

  if (m_valves.size() >= MaxValves) {
    throw std::runtime_error("Too many valves");
  }

  open_valves_mask = (1 << valves_to_open) - 1;
}



template<size_t N>
struct Position {
  std::array<CompactName, N> values;

  explicit Position(CompactName name) { values.fill(name); }

  CompactName & operator[](size_t n) { return values[n]; }
  CompactName operator[](size_t n) const { return values[n]; }

  bool operator<(Position other) const {
    auto lhs = values;
    std::sort(lhs.begin(), lhs.end());

    auto rhs = other.values;
    std::sort(rhs.begin(), rhs.end());

    for (size_t i = 0; i != N; ++i) {
      if (lhs[i] < rhs[i]) return true;
      if (lhs[i] > rhs[i]) return false;
    }
    return false;
  }

  bool operator>(Position other) const { return other < *this; }

  bool operator==(Position other) const {
    auto lhs = values;
    std::sort(lhs.begin(), lhs.end());

    auto rhs = other.values;
    std::sort(rhs.begin(), rhs.end());

    return lhs == rhs;
  }
};


template<size_t N>
struct State {
  std::bitset<MaxNonEmptyValves> oppened;
  long int total_pressure = 0;

  Position<N> elephant_pos;

  explicit State(CompactName name) : elephant_pos(name) {}

  void next_states(
    std::vector<State> & output,
    std::vector<State> & temporary_states,
    const Valves & valves, int remaining) const {

    size_t segment_start = temporary_states.size();
    temporary_states.emplace_back(*this);
    size_t segment_end = temporary_states.size();

    for (size_t i = 0; i != N; ++i) {
      for (size_t x = segment_start; x != segment_end; ++x) {
        /* copy */ const State state = temporary_states[x];
        state.push_next_states(
          i + 1 == N ? output : temporary_states, valves, remaining, i
        );
      }

      segment_start = segment_end;
      segment_end = temporary_states.size();
    }
  }



  void push_next_states(
    std::vector<State> & output,
    const Valves & valves, int remaining,
    size_t n
  ) const {
    const Valve & valve = valves[elephant_pos[n]];

    if (valve.flow_rate != 0 && !oppened.test(elephant_pos[n])) {
      State copy = *this;
      copy.oppened.set(elephant_pos[n]);
      if (remaining >= 0)
        copy.total_pressure += remaining * valve.flow_rate;
      output.emplace_back(copy);
    }

    for (const CompactName & next_valve : valve.lead_to.get_all()) {
      State copy = *this;
      copy.elephant_pos[n] = next_valve;
      output.emplace_back(copy);
    }
  }

  long int get_astar_estimate(const Valves & valves, int remaining_days) const {
    long int pressure = total_pressure;
    if (remaining_days <= 0) return pressure;

    bad_static_vector<int, MaxNonEmptyValves> remaining_energy;
    
    for (size_t i = 0; i != valves.number_of_valves_to_open(); ++i) {
      if (!oppened.test(i)) {
        remaining_energy.add(valves[i].flow_rate);
      }
    }

    const auto remaining_span = remaining_energy.get_all();
    std::sort(remaining_span.rbegin(), remaining_span.rend());

    auto cur = remaining_span.begin();
    auto end = remaining_span.end();

    while (remaining_days > 0) {
      for (size_t i = 0; i != N; ++i) {
        if (cur == end) return pressure;
        pressure += *cur * remaining_days;
        ++cur;
      }

      if (cur == end) return pressure;
      remaining_days -= 2;
    }

    return pressure;
  }
};


template<size_t N>
bool order_states(const State<N> & lhs, const State<N> & rhs, const unsigned long all_open_mask) {
  const auto lhs_op = lhs.oppened.to_ulong();
  const auto rhs_op = rhs.oppened.to_ulong();
  if (lhs_op < rhs_op) return false;
  if (lhs_op > rhs_op) return true;

  if (lhs_op != all_open_mask) {
    if (lhs.elephant_pos < rhs.elephant_pos) return true;
    if (lhs.elephant_pos > rhs.elephant_pos) return false;
  }

  if (lhs.total_pressure > rhs.total_pressure) return true;
  if (lhs.total_pressure < rhs.total_pressure) return false;

  return false;
}

template<size_t N>
bool rhs_is_useless(const State<N> & lhs, const State<N> & rhs, const unsigned long all_open_mask) {
  return lhs.oppened == rhs.oppened 
    && (lhs.oppened.to_ulong() == all_open_mask || lhs.elephant_pos == rhs.elephant_pos);
}


template<size_t N>
void remove_duplicates_unique(
  std::vector<State<N>> & current_states, 
  unsigned long all_open_mask
) {
  const auto end = std::unique(
    current_states.begin(),
    current_states.end(),
    [&](const State<N> & lhs, const State<N> & rhs) {
      return rhs_is_useless(lhs, rhs, all_open_mask);
    }
  );

  current_states.erase(end, current_states.end());
}

#include <list>
template<size_t N>
void remove_duplicates_list(
  std::vector<State<N>> & current_states, 
  unsigned long all_open_mask
) {
  if (current_states.size() <= 1) return;

  std::list<State<N>> list(current_states.begin(), current_states.end());

  auto it_a = list.begin();
  auto it_b = list.begin();
  ++it_b;

  while (it_b != list.end()) {
    if (rhs_is_useless(*it_a, *it_b, all_open_mask)) {
      it_b = list.erase(it_b);
    } else {
      ++it_a;
      ++it_b;
    }
  }

  current_states.clear();
  current_states.insert(current_states.begin(), list.begin(), list.end());
}


template<size_t N>
long int get_max_pressure(const std::vector<State<N>> & states) {
  return std::max_element(states.begin(), states.end(),
    [](const State<N> & lhs, const State<N> & rhs) {
      return lhs.total_pressure < rhs.total_pressure;
    }
  )->total_pressure;
}

template<size_t N>
void astar_pruning(std::vector<State<N>> & states, const Valves & valves, const int current_minute) {
  const auto max_pressure = get_max_pressure(states);

  std::erase_if(states, [&](const State<N> & state) {
    return state.get_astar_estimate(valves, 30 - current_minute - 1) < max_pressure;
  });
}

template<size_t N>
void duplicate_states_pruning(std::vector<State<N>> & states, const unsigned long all_open_mask) {
  std::sort(states.begin(), states.end(),
    [&](const State<N> & lhs, const State<N> & rhs) {
      return order_states(lhs, rhs, all_open_mask);
    }
  );

  remove_duplicates_unique(states, all_open_mask);
}


template<size_t N>
long int solve(const Valves & valves, int start_i) {
  const auto all_open_mask = valves.get_open_valve_mask();  

  std::vector<State<N>> current_states;
  std::vector<State<N>> temporary_states;
  current_states.emplace_back(valves.get_id_of("AA"));


  // const auto start = std::chrono::high_resolution_clock::now();
  std::vector<State<N>> next;
  for (int i = start_i; i != 30; ++i) {
    astar_pruning(current_states, valves, i);
    duplicate_states_pruning(current_states, all_open_mask);

    // Ok let's go
    // const auto current_time = std::chrono::high_resolution_clock::now();
    // const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start).count();
    // std::cout << i << '[' << duration << ']' << " " << current_states.size() << "\n";
    
    next.clear();
    temporary_states.clear();
    for (const State<N> & s : current_states) {
      if (s.oppened.to_ulong() == all_open_mask) {
        next.emplace_back(s);
        continue;
      }

      s.next_states(next, temporary_states, valves, 30 - i - 1);
    }

    if (next.size() == 0) {
      next.emplace_back(current_states[0]);
    }

    std::swap(current_states, next);
  }

  return get_max_pressure(current_states);
}

}

Output day_2022_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Valves valves(lines);

  const auto part_a = solve<1>(valves, 0);
  const auto part_b = solve<2>(valves, 4);  

  return Output(part_a, part_b);
}
