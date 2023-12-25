#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include "../../util/instruction_reader.hpp"
#include <set>
#include "../../util/position.hpp"
#include <compare>
#include <ranges>
#include <algorithm>
#include <format>

using i64 = std::int64_t;
using Grid = std::vector<std::string>;

// https://adventofcode.com/2023/day/25


/* Fill the group with all nodes that nodes in the group are connected to*/
void complete(
  std::set<std::string> & group,
  const std::map<std::string, std::set<std::string>> & wires
) {
  while (true) {
    std::set<std::string> copy = group;

    for (const std::string & wire : copy) {
      const auto it = wires.find(wire);
      if (it == wires.end()) continue;

      for (const std::string & co : it->second) {
        group.emplace(co);
      }
    }

    if (copy.size() == group.size()) return;
  }
}

/* Check if the nodes are in two disjoint groups */
i64 count(
  const std::map<std::string, std::set<std::string>> & wires,
  const std::set<std::string> & all
) {
  std::set<std::string> group_1;
  group_1.emplace(*all.begin());

  complete(group_1, wires);

  if (group_1.size() == all.size()) return 0;

  const auto it = std::find_if(
    all.begin(), all.end(),
    [&](const std::string & s) {
      return !group_1.contains(s);
    }
  );

  if (it == all.end()) return 0;

  const std::string & other = *it;

  std::set<std::string> group_2;
  group_2.emplace(other);

  complete(group_2, wires);

  if (group_1.size() + group_2.size() != all.size()) return 0;

  return group_1.size() * (all.size() - group_1.size());
}


Output day_2023_25(const std::vector<std::string> & lines, const DayExtraInfo &) {
  return Output("This file does not work and is not worth fixing, run the python script js/2023_25.py", 0);

  std::map<std::string, std::set<std::string>> wires;
  std::set<std::string> all;
  std::set<std::pair<std::string, std::string>> all_wires;

  for (const std::string & line : lines) {
    std::vector<std::string> v = bj::string_split(line, " ");
    v[0] = v[0].substr(0, v[0].size() - 1);
    std::string me = v[0];
    v.erase(v.begin());

    for (const std::string & wire : v) {
      wires[v[0]].emplace(wire);
      wires[wire].emplace(v[0]);
      all.emplace(wire);

      if (v[0] < wire)
        all_wires.emplace(v[0], wire);
      else
        all_wires.emplace(wire, v[0]);
    }
  }


  std::vector<std::pair<std::string, std::string>> all_wires_vec(all_wires.begin(), all_wires.end());

  const auto remove_wire = [&](std::pair<std::string, std::string> & p) {
    wires[p.first].erase(p.second);
    wires[p.second].erase(p.first);
  };

  const auto add_wire = [&](std::pair<std::string, std::string> & p) {
    wires[p.first].emplace(p.second);
    wires[p.second].emplace(p.first);
  };

  if (all_wires.size() >= 200) {
    return Output("Puzzle to big to be solved, run the python script", 0);
  }

  for (size_t i = 0; i < all_wires_vec.size(); ++i) {
    for (size_t j = i + 1; j < all_wires_vec.size(); ++j) {
      for (size_t k = j + 1; k < all_wires_vec.size(); ++k) {
        remove_wire(all_wires_vec[i]);
        remove_wire(all_wires_vec[j]);
        remove_wire(all_wires_vec[k]);

        i64 r = count(wires, all);
        if (r != 54) {
          return Output(r, "!");
        }

        add_wire(all_wires_vec[i]);
        add_wire(all_wires_vec[j]);
        add_wire(all_wires_vec[k]);
      }
    }
  }

  return Output(0, 0);
}
