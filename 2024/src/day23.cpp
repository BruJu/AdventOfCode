#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <map>
#include <utility>
#include <variant>
#include <set>
#include <ranges>
#include <algorithm>

#include <ctime>
#include <random>

// https://adventofcode.com/2024/day/23

using i64 = std::int64_t;

namespace {


int do_part_a(std::map<std::string, std::set<std::string>> & links) {
  int part_a = 0;

  for (const auto & [a, bs] : links) {
    for (const std::string & b : bs) {
      if (a >= b) continue;

      for (const auto & c : links[b]) {
        if (a >= c) continue;
        if (b >= c) continue;

        if (!bs.contains(c)) continue;

        if (a[0] == 't' || b[0] == 't' || c[0] == 't') {
          if (links.size() < 50) {
            printf("%s %s %s\n", a.c_str(), b.c_str(), c.c_str());
          }
          ++part_a;
        }
      }

    }
  }

  return part_a;
}

std::vector<std::set<std::string>> grow(
  const std::vector<std::set<std::string>> & previous_cliques,
  std::map<std::string, std::set<std::string>> & links
  ) {
  std::vector<std::set<std::string>> new_cliques;

  for (const std::set<std::string> & clique : previous_cliques) {
    const std::string elem = *clique.begin();

    for (const std::string & other : links[elem]) {
      if (clique.contains(other)) continue;

      if (std::any_of(clique.begin(), clique.end(),
        [&](const std::string & e) {
          return !links[e].contains(other);
        }
      )) continue;

      auto copy = clique;
      copy.emplace(other);
      new_cliques.emplace_back(copy);
    }
  }
  
  printf("%zu\n", new_cliques.size());

  return new_cliques;
}


std::string do_part_b(std::map<std::string, std::set<std::string>> & links) {

  std::vector<std::set<std::string>> cliques;

  for (const auto & [a, _] : links) {
    std::set<std::string> singleton;
    singleton.emplace(a);
    cliques.emplace_back(singleton);
  }

  if (cliques.empty()) return "";

  while (true) {
    if (cliques.size() > 1000) {
      return "let's move on to the next puzzle";
    }

    std::vector<std::set<std::string>> new_cliques = grow(cliques, links);

    if (new_cliques.size() == 0) {
      std::string names;
      for (const std::string & s : *cliques.begin()) {
        names += ",";
        names += s;
      }
      return names.substr(1);
    }

    cliques = std::move(new_cliques);
  }
}










}

Output day_2024_23(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::map<std::string, std::set<std::string>> links;

  for (const std::string & line : lines) {
    std::string a = line.substr(0, 2);
    std::string b = line.substr(3);

    
    links[a].emplace(b);
    links[b].emplace(a);
  }

  int part_a = do_part_a(links);

  std::string part_b = do_part_b(links);




  return Output(
    part_a, part_b
    );
}
