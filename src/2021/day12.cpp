#include "../advent_of_code.hpp"
#include <set>
#include <vector>
#include "../util/position.hpp"

// https://adventofcode.com/2021/day/12

/** Returns true if c is an uppercase character */
static bool is_uppercase(char c) {
  return c >= 'A' && c <= 'Z';
}

/** Info related to a cave */
struct Cave {
  std::string name;
  std::vector<Cave *> connected_to;

  explicit Cave(std::string name) : name(std::move(name)) {}

  [[nodiscard]] bool is_small() const {
    return std::find_if(name.begin(), name.end(), is_uppercase) == name.end();
  }
};

/** Enums conveys more semantic than booleans as parameters */
enum class AllowShadowVisit { One, No };

/** A path exploration */
struct Dora_D_Explorer {
  std::string sequence_name;
  std::set<std::string> visited_smalls;
  bool used_shadow_visit;
  const Cave * current_position;

  explicit Dora_D_Explorer(const Cave & start, AllowShadowVisit allowShadowVisit) {
    sequence_name = start.name;

    if (start.is_small()) {
      visited_smalls.emplace(start.name);
    }

    used_shadow_visit = allowShadowVisit == AllowShadowVisit::No;

    current_position = &start;
  }

private:
  Dora_D_Explorer(const Dora_D_Explorer & previous, const Cave * towards, bool shadow_visit) {
    used_shadow_visit = shadow_visit || previous.used_shadow_visit;

    sequence_name = previous.sequence_name + '-' + towards->name;
    
    visited_smalls = previous.visited_smalls;
    if (towards->is_small() && !shadow_visit) {
      visited_smalls.emplace(towards->name);
    }

    current_position = towards;
  }

public:
  template <typename CanReachTheEnd, typename OnNewPath>
  void for_each_next_path(CanReachTheEnd canReachTheEnd, OnNewPath onNewPath) const {
    for (const Cave * const next : current_position->connected_to) {
      if (next->name == "start") {
        /* noop */
      } else if (next->name == "end") {
        canReachTheEnd();
      } else {
        if (next->is_small()) {
          if (visited_smalls.find(next->name) != visited_smalls.end()) {
            continue;
          }

          if (!used_shadow_visit) {
            onNewPath(Dora_D_Explorer(*this, next, true));
          }
        }

        onNewPath(Dora_D_Explorer(*this, next, false));
      }
    }
  }
};

struct Explorations {
  std::set<std::string> reached_the_end;
  std::vector<Dora_D_Explorer> todo;

  explicit Explorations(const Cave & start, AllowShadowVisit allowShadowVisit) {
    todo.emplace_back(start, allowShadowVisit);
  }

  void find_all_paths_to_the_end() {
    while (!todo.empty()) {
      const auto dora = todo.back();
      todo.pop_back();

      dora.for_each_next_path(
        // Terminal
        [&]() { reached_the_end.emplace(dora.sequence_name); },
        // New path to explore
        [&](Dora_D_Explorer path) { todo.emplace_back(std::move(path)); }
      );
    }
  }
};

/** Transform the input into a list of caves */
static std::vector<Cave> build_network(const std::vector<std::string> & lines) {
  // Read list of caves + connections
  std::set<std::string> names;
  std::vector<std::pair<std::string, std::string>> connections;

  for (const auto & line : lines) {
    if (line == "") continue;
    const auto separator_index = line.find('-');
    if (separator_index == std::string::npos) {
      std::cerr << "No separator on line [" << line << "]\n";
      continue;
    }

    const auto from = line.substr(0, separator_index);
    const auto to = line.substr(separator_index + 1);

    names.emplace(from);
    names.emplace(to);
    connections.emplace_back(std::pair(from, to));
  }

  // Put the caves in an std::vector
  std::vector<Cave> caves;
  for (const auto & name : names) {
    caves.push_back(Cave(name));
  }
  
  // Now we never push_back anymore in the std::vector -> The Cave pointers
  // are stable

  for (const auto & connection : connections) {
    // We don't use structured binding because we can't capture variables that
    // comes from them
    const auto & from = connection.first;
    const auto & to = connection.second;
    const auto from_it = std::find_if(caves.begin(), caves.end(), [&](const Cave & cave) { return cave.name == from; });
    const auto to_it   = std::find_if(caves.begin(), caves.end(), [&](const Cave & cave) { return cave.name == to  ; });

    from_it->connected_to.push_back(&*to_it);
    to_it->connected_to.push_back(&*from_it);
  }

  return caves;
}

Output day_2021_12(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<Cave> caves = build_network(lines);
  
  const Cave & start = *std::find_if(
    caves.begin(), caves.end(), 
    [](const Cave & cave) { return cave.name == "start"; }
  );

  Explorations part_a(start, AllowShadowVisit::No);
  part_a.find_all_paths_to_the_end();

  Explorations part_b(start, AllowShadowVisit::One);
  part_b.find_all_paths_to_the_end();

  return Output(part_a.reached_the_end.size(), part_b.reached_the_end.size());
}
