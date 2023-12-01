#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include "../../util/position.hpp"

// Day 17 was boring, days 18 and 19 were too long, today is
// "I'm very smart with my non representative only example"

static std::vector<bool> compute_enchancement(const std::string & s) {
  std::vector<bool> r;
  for (const char c : s) {
    r.push_back(c == '#');
  }
  return r;
}

struct Grid {
  std::map<bj::Position, bool> pixels;
  bool padding;
  const std::vector<bool> * enhancement;

  Grid(const std::vector<bool> * enhancement, std::set<bj::Position> lit)
  : padding(false), enhancement(enhancement) {
    for (const auto & position : lit) {
      pixels.emplace(position, true);
    }
  }

  Grid next() const {
    Grid n;
    n.enhancement = enhancement;
    n.padding = padding ? enhancement->back() : enhancement->front();

    const auto consider = [&](bj::Position pos) {
      int enhance_code = 0;
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          enhance_code = enhance_code * 2;
          
          auto it = pixels.find(bj::Position{ pos.x + dx, pos.y + dy });
          bool is_lit;
          if (it == pixels.end()) {
            is_lit = padding;
          } else {
            is_lit = it->second;
          }

          if (is_lit) {
            enhance_code += 1;
          }
        }
      }

      bool will_be_lit = (*enhancement)[enhance_code];

      if (n.padding || will_be_lit) n.pixels.emplace(pos, will_be_lit);
    };

    for (const auto & [position, is_lit] : pixels) {
      consider(position);

      for (const auto & neighbour : position.get_8_neighbours()) {
        consider(neighbour);
      }
    }

    return n;
  }


private:
  Grid() = default;
};

Output day_2021_20(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<bool> image_enhancement = compute_enchancement(lines[0]);

  std::set<bj::Position> origin;

  for (size_t line = 0; line + 2 != lines.size(); ++line) {
    for (size_t col = 0; col != lines[line + 2].size(); ++col) {
      if (lines[line + 2][col] == '#') {
        origin.emplace(bj::Position{ int(col), int(line) });
      }
    }
  }

  Grid grid(&image_enhancement, origin);

  for (int i = 0; i != 2; ++i) {
    grid = grid.next();
  }

  const long long int part_a = grid.pixels.size();

  for (int i = 2; i != 50; ++i) {
    grid = grid.next();
  }

  const long long int part_b = grid.pixels.size();

  return Output(part_a, part_b);
}
