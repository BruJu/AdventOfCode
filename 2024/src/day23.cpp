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
#include "../../util/libs_ensemblist.hpp"

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
          // if (links.size() < 50) {
          //   printf("%s %s %s\n", a.c_str(), b.c_str(), c.c_str());
          // }
          ++part_a;
        }
      }

    }
  }

  return part_a;
}

// https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
void
bronKerbosch1(
  std::vector<std::set<std::string>> & maximal_cliques,
  std::set<std::string> R,
  std::set<std::string> P,
  std::set<std::string> X,
  std::map<std::string, std::set<std::string>> & links
) {
  if (P.empty() && X.empty()) {
    maximal_cliques.emplace_back(R);
    return;
  }

  for (auto it_P = P.begin(); it_P != P.end(); ++it_P) {
    const std::string & vertex = *it_P;

    // 1- Prepare recursive call

    std::set<std::string> next_R = R; next_R.emplace(vertex);
    
    std::set<std::string> next_P(it_P, P.end());
    std::set<std::string> next_X = X;
    
    auto it_links = links.find(vertex);
    if (it_links != links.end()) {
      next_P &= it_links->second;
      next_X &= it_links->second;
    } else {
      next_P.clear();
      next_X.clear();
    }

    // 2- Recursive call
    bronKerbosch1(
      maximal_cliques,
      next_R, next_P, next_X, links
    );

    // 3- Move on

    // P.erase(vertex); -> this is performed automtically because we only consider [it_P, P.end() [
    X.emplace(vertex);
  }
}

std::string do_part_b(std::map<std::string, std::set<std::string>> & links) {
  // 1- Prepare parameters for Bron Kerbosch  
  std::vector<std::set<std::string>> maximal_cliques;
  std::set<std::string> R;
  std::set<std::string> P;
  std::set<std::string> X;

  for (const auto & link : links) {
    P.emplace(link.first);
  }

  // 2- Bron Kerbosch call
  bronKerbosch1(maximal_cliques, R, P, X, links);

  // 3- Compute output
  std::set<std::string> biggest_clique;
  for (const auto & clique : maximal_cliques) {
    if (clique.size() > biggest_clique.size()) {
      biggest_clique = clique;
    }
  }

  std::string str;
  for (const std::string & node : biggest_clique) {
    if (!str.empty()) str += ',';
    str += node;
  }

  return str;
}

}

Output day_2024_23(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::map<std::string, std::set<std::string>> links;

  for (const std::string & line : lines) {
    std::string a = line.substr(0, 2);
    std::string b = line.substr(3);

    links[a].emplace(b);
    links[b].emplace(a);
  }

  const int part_a = do_part_a(links);
  const std::string part_b = do_part_b(links);

  return Output(part_a, part_b);
}
