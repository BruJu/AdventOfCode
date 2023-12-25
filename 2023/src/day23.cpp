#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include "../../util/instruction_reader.hpp"
#include <set>
#include "../../util/position.hpp"
#include <compare>
#include <ranges>
#include <algorithm>

using i64 = std::int64_t;
using Grid = std::vector<std::string>;

// https://adventofcode.com/2023/day/23

struct Path {
  bj::Position start;
  bj::Position end;
  i64 steps;

  friend std::ostream & operator<<(std::ostream & s, const Path & path) {
    return s << '(' << path.start.x << ',' << path.start.y << ')'
    << " => (" << path.end.x << ',' << path.end.y << ") in " << path.steps;
  }

  friend std::ostream & operator<<(std::ostream & s, const std::vector<Path> & paths) {
    for (const Path & path : paths) {
      s << path << "\n";
    }
    return s;
  }

  [[nodiscard]] constexpr auto operator<=>(const Path &) const noexcept = default;
};

struct Cartography {
  std::vector<Path> paths;
  std::map<bj::Position, std::vector<size_t>> paths_from;

  Cartography(std::vector<Path> paths) : paths(paths) {
    for (size_t i = 0; i != paths.size(); ++i) {
      paths_from[paths[i].start].emplace_back(i);
    }
  }

  std::span<const size_t> find_starts(bj::Position pos) const {
    const auto it = paths_from.find(pos);
    if (it == paths_from.end()) return {};
    return it->second;
  }

  [[maybe_unused]] void print_graphviz(const bj::Position target) const {
    std::map<std::string, int> node_to_level;

    const auto get_node_name = [&](const bj::Position & path) -> std::string {
      if (path == target) return "end";
      if (path == bj::Position{ 1, 0 }) return "start";
      std::string x = "node_";
      x += std::to_string(path.x);
      x += "_";
      x += std::to_string(path.y);
      return x;
    };

    node_to_level["start"] = 0;

    while (true) {
      bool stable = true;

      for (const auto & path : paths) {
        if (path.end < path.start) continue;
        std::string start = get_node_name(path.start);
        std::string end = get_node_name(path.end);

        bool has_start = node_to_level.contains(start);
        bool has_end = node_to_level.contains(end);
        if (has_start && has_end) {
          continue;
        } else if (!has_start && !has_end) {
          continue;
        } else if (has_start) {
          node_to_level[end] = node_to_level[start] + 1;
          stable = false;
        } else {
          node_to_level[start] = node_to_level[end] + 1;
          stable = false;
        }
      }

      if (stable) break;
    }

    for (const auto & path : paths) {
      if (path.end < path.start) continue;

      std::string start = get_node_name(path.start);
      std::string end = get_node_name(path.end);

      std::cout << start << " -> " << end << " [label=\"" << path.steps << "\"]\n"; 
    }

    int rank = 0;
    while (true) {
      std::vector<std::string> nodes;

      for (const auto & [name, his_rank] : node_to_level) {
        if (his_rank == rank) {
          nodes.emplace_back(name);
        }
      }

      if (nodes.empty()) break;
      
      std::cout << "{rank = same; ";

      for (const std::string & name : nodes) {
        std::cout << name << "; ";
      }
      std::cout << "}\n";

      ++rank;
    }
  }
};


std::vector<Path> read_paths(const std::vector<std::string> & lines) {
  struct PathFinder {
    std::vector<std::string> lines;
    std::vector<Path> paths;

    bool add_path(bj::Position from, bj::Position to, i64 steps) {
      Path path{ from, to, steps };

      const auto it = std::find_if(
        paths.begin(), paths.end(),
        [&](const Path & other) {
          return other == path;
        }
      );

      if (it != paths.end()) return false;
      paths.emplace_back(path);
      return true;
    }

    void explore(bj::Kitten kitten) {
      bj::Position first_pos = kitten.position;
      kitten.advance();

      std::vector<bj::Direction> candidates;

      i64 nb_steps = 1;
      
      while (true) {
        candidates.clear();
        kitten.turn_180();
        bj::Direction forward = kitten.direction;
        kitten.turn_180();
        for (bj::Direction dir : { bj::Direction::Left, bj::Direction::Right, bj::Direction::Down, bj::Direction::Top }) {
          if (dir == forward) continue;
          kitten.direction = dir;
          kitten.advance();
          bj::Position position = kitten.position;
          kitten.turn_180();
          kitten.advance();
          
          bool the_end = position.x < 0
            || position.y < 0
            || position.x >= lines[0].size()
            || position.y >= lines.size();

          if (the_end) {
            continue;
          }

          if (position == bj::Position{ int(lines[0].size() - 2), int(lines.size() - 1) }) {
            add_path(first_pos, position, nb_steps + 1);
            continue;
          }

          char c = lines[position.y][position.x];
          if (c == '.') {
            candidates.emplace_back(dir);
          } else if (c == '#') {

          } else {
            char slope = dir == bj::Direction::Left ? '<' :
                        dir == bj::Direction::Right ? '>' :
                        dir == bj::Direction::Down ? 'v' :
                        '^';
            
            if (slope == c) {
              candidates.emplace_back(dir);
            }
          }
        }


        if (candidates.size() == 1) {
          ++nb_steps;
          kitten.direction = candidates[0];
          kitten.advance();
        } else if (candidates.size() == 0) {
          return;
        } else {
          bool b = add_path(first_pos, kitten.position, nb_steps);
          if (!b) return;

          for (const auto candidate : candidates) {
            kitten.direction = candidate;
            explore(kitten);
          }

          return;
        }
      }
    }
  };

  PathFinder finder { lines };

  bj::Kitten k{ bj::Position{ 1, 0 }, bj::Direction::Down };
  finder.explore(k);
  return finder.paths;
}


struct BestResult {
  bool has = false;
  i64 steps = 0;
//  std::vector<size_t> paths;

  void accumulate(BestResult r) {
    if (!r.has) return;

    if (!has || steps < r.steps) {
      *this = r;
      return;
    }
  }

  i64 value_or(i64 v) {
    if (has) {
      return steps;
    }
    return v;
  }
};


BestResult find_longest(
  const Cartography & cartography,
  std::span<std::uint8_t> taken,
  bj::Position current_position,
  bj::Position target
) {
  BestResult result;

  for (const size_t i_path : cartography.find_starts(current_position)) {
    if (taken[i_path] != 0) continue;

    const Path & path = cartography.paths[i_path];

    if (path.start == current_position) {
      if (path.end == target) {
        result.accumulate(BestResult{ true, path.steps /*, std::vector<size_t>{i_path} */ });
      } else {
        taken[i_path] = 1;
        for (size_t i = 0; i != cartography.paths.size(); ++i) {
          if (cartography.paths[i].end == path.start) {
            ++taken[i];
          }
        }

        BestResult temp = find_longest(cartography, taken, path.end, target);
        temp.steps += path.steps;
        result.accumulate(temp);
        
        for (size_t i = 0; i != cartography.paths.size(); ++i) {
          if (cartography.paths[i].end == path.start) {
            --taken[i];
          }
        }

        taken[i_path] = 0;
      }
    }
  }

  return result;
}


void remove_slopes(Grid & grid) {
  for (std::string & s : grid) {
    for (char & c : s) {
      if (c != '#') c = '.';
    }
  }
}


Output day_2023_23(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const bj::Position target { int(lines[0].size() - 2), int(lines.size() - 1) };

  i64 part_a = 0;
  {
    std::vector<Path> paths = read_paths(lines);
    std::vector<std::uint8_t> taken(paths.size(), 0);
    part_a = find_longest(paths, taken, bj::Position{ 1, 0 }, target).value_or(0);
  }

  i64 part_b = 0;
  {
    // For Part B, we just bruteforce by removing some paths that are
    // obviously useless.
    auto copy = lines;
    remove_slopes(copy);


    std::vector<Path> paths = read_paths(copy);
    std::vector<std::uint8_t> taken(paths.size(), 0);

    Cartography carto(paths);
    
    const auto remove_path = [&](std::string from, std::string to) {
      size_t erased = 0;
      auto it = paths.begin();

      while (it != paths.end()) {
        std::string a = std::format("node_{}_{}", it->start.x, it->start.y);
        std::string b = std::format("node_{}_{}", it->end.x, it->start.y);

        bool remove = (from == a && to == b) || (to == a && from == b);
        if (remove) {
          it = paths.erase(it);
          ++erased;
        } else {
          ++it;
        }
      }

      if (erased != 2) {
        throw std::runtime_error("did not erase enough");
      }
    };
    

    bj::Position target_ = target;
    int add = 0;

    if (dei.part_a_extra_param == 1) {
      target_ = bj::Position{ 135, 137 };
      add = 15;

      remove_path("node_111_137", "node_135_137");
      remove_path("node_11_19", "node_9_31");

      remove_path("node_105_9", "node_105_37");
      remove_path("node_105_37", "node_129_33");
    }

    part_b = find_longest(paths, taken, bj::Position{ 1, 0 }, target_).value_or(0) + add;
  }

  return Output(part_a, part_b);
}
