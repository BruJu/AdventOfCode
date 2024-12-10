#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <set>
#include <utility>

// https://adventofcode.com/2024/day/10

namespace {

void hike_recursive(
  const std::vector<std::string> & lines,
  bj::Position position,
  char next_step,
  std::set<bj::Position> & out_reachable,
  int & out_distinct_paths
) {
  position.for_each_neighbour([&](bj::Position next) {
    if (next.is_out_of_bound(lines)) return;

    if (lines[next.y][next.x] == next_step) {
      if (next_step == '9') {
        out_reachable.emplace(next);
        ++out_distinct_paths;
      } else {
        hike_recursive(lines, next, next_step + 1, out_reachable, out_distinct_paths);
      }
    }
  });
}

std::pair<std::set<bj::Position>, int> hike(
  const std::vector<std::string> & lines,
  bj::Position position
) {
  std::pair<std::set<bj::Position>, int> output;
  hike_recursive(lines, position, '0' + 1, output.first, output.second);
  return output;
}

}

Output day_2024_10(const std::vector<std::string> & lines, const DayExtraInfo &) {
  int total_trailscore = 0;
  int distinct_paths = 0;

  for (size_t y = 0; y != lines.size(); ++y) {
    for (size_t x = 0; x != lines[y].size(); ++x) {
      if (lines[y][x] == '0') {
        // This is a trailhead
        const auto [reachable, local_distinct_paths] = hike(lines, bj::Position::from(x, y));

        total_trailscore += reachable.size();
        distinct_paths += local_distinct_paths;
      }
    }
  }

  return Output(total_trailscore, distinct_paths); 
}
