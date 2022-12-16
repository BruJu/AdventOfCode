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
#include <span>
// https://adventofcode.com/2022/day/16


using CompactName = int;
int compact_name(const std::string & s) { return s[0] * 0x100 + s[1]; }

struct Valve {
  int flow_rate;
  std::vector<CompactName> lead_to;
};

struct BigPos {
  CompactName me;
  CompactName phanpy;

  BigPos(CompactName a, CompactName b) {
    if (a > b) {
      me = b;
      phanpy = a;
    } else {
      me = a;
      phanpy = b;
    }
  }

  bool operator<(const BigPos & other) const {
    if (me < other.me) return true;
    if (me > other.me) return false;
    if (phanpy < other.phanpy) return true;
    return false;
  }
  bool operator==(const BigPos & other) const {
    return me == other.me && phanpy == other.phanpy;
  }
  bool operator!=(const BigPos & other) const { return !(operator==(other)); }
  bool operator>(const BigPos & other) const {
    return !(operator<(other)) && !(operator==(other));
  }

};

struct Valves {
private:
  std::map<CompactName, Valve> m_valves;
  size_t valves_to_open;

public:
  explicit Valves(const std::vector<std::string> & lines);

  [[nodiscard]] const Valve & operator[](const CompactName name) const {
    return m_valves.find(name)->second;
  }

  [[nodiscard]] size_t number_of_valves_to_open() const noexcept {
    return valves_to_open;
  }

  auto get_valves() const { return m_valves; }
};

Valves::Valves(const std::vector<std::string> & lines) {
  std::regex regex_ { R"(Valve ([A-Z][A-Z]) has flow rate=(\d+); tunnels? leads? to valves? (.*))"};
  std::smatch smatch_;

  for (const auto & line : lines) {
    if (!std::regex_search(line, smatch_, regex_)) {
      throw std::runtime_error(":(");
    }

    const CompactName name = compact_name(smatch_[1].str());

    Valve valve;
    valve.flow_rate = std::stoi(smatch_[2].str());
    std::vector<std::string> next_name = bj::string_split(smatch_[3].str(), ", ");
    for (const auto & name : next_name) {
      valve.lead_to.emplace_back(compact_name(name));
    }

    m_valves[name] = std::move(valve);
  }

  // Cache the number of valves to open
  valves_to_open = 0;
  for (const Valve & valve : m_valves | std::views::values) {
    if (valve.flow_rate != 0) {
      ++valves_to_open;
    }
  }
}

struct OpenedCacheId {
  size_t id;

  bool contains(CompactName name) const;
  size_t size() const;
  const std::set<CompactName> & get() const;
};

std::map<size_t /*  OpenedCacheId*/, int> left_energy;

struct OpenedCache {
  std::vector<std::set<CompactName>> pos;

  OpenedCache() {
    pos.emplace_back(std::set<CompactName>{});
  }

  OpenedCacheId get_id_for(const std::set<CompactName> & names) {
    for (size_t i = 0; i != pos.size(); ++i) {
      if (pos[i] == names) return OpenedCacheId{ i };
    }

    pos.emplace_back(names);
    return OpenedCacheId { pos.size() - 1 };
  }

  const std::set<CompactName> & get(OpenedCacheId id) {
    return pos[id.id];
  }
};

OpenedCache cache_for_opened;

bool OpenedCacheId::contains(CompactName name) const {
  return cache_for_opened.get(*this).contains(name);
}

size_t OpenedCacheId::size() const { return cache_for_opened.get(*this).size(); }
  
const std::set<CompactName> & OpenedCacheId::get() const {
  return cache_for_opened.get(*this);
}

int left_eneergy(const Valves & valves, OpenedCacheId openedid) {
  auto it = left_energy.find(openedid.id);
  if (it != left_energy.end()) return it->second;

int score = 0;
    for (const auto & [name, valve] : valves.get_valves()) {
      if (valve.flow_rate != 0) {
        if (openedid.contains(name)) {
        } else {
          score += valve.flow_rate;
        }
      }
    }
left_energy.emplace(openedid.id, score);
    return score;

}

struct State {
  struct Partial {
    OpenedCacheId oppened;
    long long int total_pressure = 0;
    CompactName me;
  };

  OpenedCacheId oppened;
  long long int total_pressure = 0;

  BigPos on_name = BigPos(compact_name("AA"), compact_name("AA"));

  std::vector<State> next_states(const Valves & valves, int remaining) const {
    // me
    std::vector<Partial> next_partial;

    const Valve & valve = valves[on_name.me];
    if (valve.flow_rate != 0 && !oppened.contains(on_name.me)) {
      auto copy_set = cache_for_opened.get(oppened);
      copy_set.emplace(on_name.me);
      
      Partial copy = { cache_for_opened.get_id_for(copy_set), total_pressure, on_name.me };
      if (remaining >= 0)
        copy.total_pressure += remaining * valve.flow_rate;
      next_partial.emplace_back(copy);
    }

    for (const CompactName & next_valve : valve.lead_to) {
      Partial copy = { oppened, total_pressure, next_valve };
      next_partial.emplace_back(copy);
    }
    return next_states_partial(next_partial, on_name.phanpy, valves, remaining);
  }

  static std::vector<State> next_states_partial(
    std::span<Partial> next_partial,
    CompactName phanpy,
    const Valves & valves, int remaining
  ) {
    std::vector<State> next_;
   
    for (const Partial & partial : next_partial) {
      const Valve & valve = valves[phanpy];

      if (valve.flow_rate != 0 && !partial.oppened.contains(phanpy)) {
        auto copy_set = cache_for_opened.get(partial.oppened);
        copy_set.emplace(phanpy);

        State copy = { cache_for_opened.get_id_for(copy_set), partial.total_pressure, 
          BigPos(partial.me, phanpy)
         };
        
        if (remaining >= 0)
          copy.total_pressure += remaining * valve.flow_rate;
        next_.emplace_back(copy);
      }

      for (const CompactName & next_valve : valve.lead_to) {
        State copy = { partial.oppened, partial.total_pressure, BigPos(partial.me, next_valve) };
        next_.emplace_back(copy);
      }
    }

    return next_;
  }

  int get_best_possible(const Valves & valves, int remaining) const {
    return left_eneergy(valves, oppened) * remaining + total_pressure;
  }

};



Output day_2022_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Valves valves(lines);
  const size_t valves_to_open = valves.number_of_valves_to_open();

  std::vector<State> current_states;
  current_states.emplace_back(State{});



  for (int i = 4; i != 30; ++i) {
    std::sort(
      current_states.begin(), current_states.end(),
      [&](const State & lhs, const State & rhs) {
        if (lhs.oppened.size() != valves_to_open) {
          if (lhs.on_name < rhs.on_name) return true;
          if (lhs.on_name > rhs.on_name) return false;
        }
        if (lhs.oppened.size() < rhs.oppened.size()) return true;
        if (lhs.oppened.size() > rhs.oppened.size()) return false;

        auto lit = lhs.oppened.get().begin();
        auto rit = rhs.oppened.get().begin();
        while (lit != lhs.oppened.get().end()) {
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
        ( lhs.oppened.size() == rhs.oppened.size() && lhs.oppened.size() == valves_to_open )
        || ( lhs.on_name == rhs.on_name && lhs.oppened.id == rhs.oppened.id)
        ) {
          listed_next = listed.erase(listed_next);
      } else {
        ++listed_it;
        ++listed_next;
      }
    }

    int best = 0;
    for (const State & state : listed) {
      if (state.total_pressure > best) best = state.total_pressure;
    }

  size_t trimmed = 0;
    auto it = listed.begin();
    while (it != listed.end()) {
      if (it->get_best_possible(valves, 30 - i - 1) < best) {
        it = listed.erase(it);
        ++trimmed;
      } else {
        ++it;
      }
    }
    std::cout << "removed " << trimmed << "\n";

    current_states = std::vector<State>(listed.begin(), listed.end());

    std::cout << i << " " << current_states.size() << "\n";
    std::vector<State> next;

    for (const State & s : current_states) {
      if (s.oppened.size() == valves_to_open) {
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
