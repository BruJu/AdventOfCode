#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <algorithm>
#include <string>
#include <span>

// https://adventofcode.com/2019/day/03

namespace {

using Int = std::int64_t;

struct Horizontal { bj::Position from; int size; };
struct Vertical   { bj::Position from; int size; };

struct Path {
  std::vector<Horizontal> horizontals;
  std::vector<Vertical> verticals;

  explicit Path(std::string str_path);

  [[nodiscard]] std::vector<bj::Position> intersect(const Path & other) const;
};

template<typename F>
void for_each_path(const std::string & str_path, F biconsumer) {
  std::vector<std::string> tokens = bj::string_split(str_path, ",");

  for (const std::string & token : tokens) {
    bj::Direction dir = bj::to_direction_from_lrtd(token[0], "LRUD").value();
    const int steps = std::stoi(token.substr(1));

    biconsumer(dir, steps);
  }
}

Path::Path(std::string str_path) {
  bj::Kitten traveller(bj::Position{ 0, 0 }, bj::Direction::Left);

  for_each_path(str_path, [&](const bj::Direction dir, const int steps) {
    traveller.direction = dir;

    if (dir == bj::Direction::Right) {
      horizontals.emplace_back(Horizontal{ traveller.position, steps });
    } else if (dir == bj::Direction::Down) {
      verticals.emplace_back(Vertical{ traveller.position, steps });
    }

    for (int i = 0; i != steps; ++i) {
      traveller.advance();
    }

    if (dir == bj::Direction::Left) {
      horizontals.emplace_back(Horizontal{ traveller.position, steps });
    } else if (dir == bj::Direction::Top) {
      verticals.emplace_back(Vertical{ traveller.position, steps });
    }
  });
}

bool is_within(int min, int value, int size) {
  return min <= value && min + size >= value;
}

void intersect_lines(
  std::vector<bj::Position> & output,
  std::span<const Horizontal> horizontals,
  std::span<const Vertical> verticals
) {
  for (const Horizontal & horizontal : horizontals) {
    for (const Vertical & vertical : verticals) {
      if (is_within(horizontal.from.x, vertical.from.x, horizontal.size)
        && is_within(vertical.from.y, horizontal.from.y, vertical.size)
      ) {
        output.emplace_back(bj::Position { vertical.from.x, horizontal.from.y });
      }
    }
  }
}

std::vector<bj::Position> Path::intersect(const Path & other) const {
  std::vector<bj::Position> result;
  intersect_lines(result, horizontals, other.verticals);
  intersect_lines(result, other.horizontals, verticals);
  // No intersection on hor or vert
  return result;
}

std::vector<int> compute_distances(std::string str_path, std::span<const bj::Position> points) {
  std::vector<int> distances;
  distances.resize(points.size(), -1);

  bj::Kitten traveller(bj::Position{ 0, 0 }, bj::Direction::Left);
  int steps = 0;

  for_each_path(str_path, [&](bj::Direction dir, int dir_steps) {
    traveller.direction = dir;
    for (int i = 0; i != dir_steps; ++i) {
      ++steps;
      traveller.advance();

      auto it = std::find(points.begin(), points.end(), traveller.position);
      if (it != points.end()) {
        size_t index = it - points.begin();
        distances[index] = distances[index] == -1 ? steps : distances[index];
      }
    }
  });

  return distances;
}

}

Output day_2019_03(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Path first(lines[0]);
  Path second(lines[1]);

  std::vector<bj::Position> intersect = first.intersect(second);
  
  // Remove leading 0,0
  auto it = std::find(intersect.begin(), intersect.end(), bj::Position{ 0, 0 });
  if (it != intersect.end()) {
    intersect.erase(it);
  }

  auto min = *std::min_element(
    intersect.begin(),
    intersect.end(),
    [](const bj::Position & lhs, const bj::Position & rhs) {
      return lhs.manhattan() < rhs.manhattan();
    }
  );

  std::vector<int> distances_first = compute_distances(lines[0], intersect);
  std::vector<int> distances_second = compute_distances(lines[1], intersect);

  int min_dist = distances_first[0] + distances_second[0];
  for (size_t i = 0; i != distances_first.size(); ++i) {
    std::cout << distances_first[i] << "-" << distances_second[i] << "\n";
    min_dist = std::min(min_dist, distances_first[i] + distances_second[i]);
  }

  return Output(min.manhattan(), min_dist);
}

