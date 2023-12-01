#include "../../common/advent_of_code.hpp"
#include "../../util/instruction_reader.hpp"
#include "../../util/position.hpp"
#include <cstring>
#include <algorithm>
#include <optional>
#include <array>
#include <set>
#include <map>
#include <queue>

struct Pos {
  bj::Position point;
  int length;
};

static bool can_walk_on(bj::Position point, int designer_number) {
  if (point.x < 0) return false;
  if (point.y < 0) return false;

  int sum =
    point.x * point.x
    + point.x * 3
    + 2 * point.x * point.y
    + point.y
    + point.y * point.y
    + designer_number;

  int number_of_1 = 0;

  while (sum != 0) {
    if (sum & 1) ++number_of_1;
    sum /= 2;
  }

  return (number_of_1 % 2) == 0;
}

enum class PointValidity { IsAnswer, DoNotExploreNeighbour, Normal };

template <typename Predicate>
static inline size_t goToPosition(int designer_number, Predicate predicate) {
  std::set<bj::Position> already_visited;
  std::queue<Pos> positions;

  positions.push(Pos{ bj::Position{1, 1}, 0 });

  while (!positions.empty()) {
    const auto pos = positions.front();
    positions.pop();

    const auto x = predicate(pos);
    if (x == PointValidity::IsAnswer) return pos.length;
    if (x == PointValidity::DoNotExploreNeighbour) continue;    

    pos.point.for_each_neighbour([&](const bj::Position & p) {
      if (!can_walk_on(p, designer_number)) return;
      if (already_visited.find(p) != already_visited.end()) return;

      positions.emplace(Pos{ p, pos.length + 1 });
      already_visited.emplace(p);
    });
  }

  return already_visited.size();
}

Output day_2016_13(const std::vector<std::string> & lines, const DayExtraInfo & extra) {
  const int target_x = std::stoi(lines[0]);
  const int target_y = std::stoi(lines[1]);
  const int designer_number = std::stoi(lines[2]);

  bj::Position target { target_x, target_y };

  const auto length = goToPosition(
    designer_number,
    [&](const Pos & pos) { return pos.point == target ? PointValidity::IsAnswer : PointValidity::Normal; }
  );

  const auto reachable = extra.can_skip_part_B ? 0 :
    goToPosition(
      designer_number,
      [&](const Pos & pos) { return pos.length > 49 ? PointValidity::DoNotExploreNeighbour : PointValidity::Normal; }
    )
  ;

  return Output(length, reachable);
}
