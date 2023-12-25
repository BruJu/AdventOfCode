#include "../../common/advent_of_code.hpp"
#include <vector>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/24


struct LineEquation {
  // y = mx + b
  double m;
  double b;

  double compute(double x) const {
    return m * x + b;
  }

  std::optional<std::pair<double, double>> intersect(const LineEquation & other) {
    if (m == other.m) return std::nullopt;
    double x = (other.b - b) / (m - other.m);
    double y = m * x + b;
    return std::pair(-x, -y);
  }
};

struct TwinkleStar {
  i64 x, y, z;
  i64 dx, dy, dz;
  
  // y = ax + b -> returns a and b
  static LineEquation get_line_equation(double x, double dx, double y, double dy) {
    double x2 = x + dx;
    double y2 = y + dy;

    double m = (y2 - y) / (x2 - x);
    double b = m * x - y;
    return LineEquation{ m, b };
  }

  static bool check_has(double x, double dx, double tx) {
    if (dx == 0) return x == tx;
    if (dx < 0) return x >= tx;
    return x <= tx;
  }

  bool has(double tx, double ty) const {
    return check_has(x, dx, tx) && check_has(y, dy, ty);
  }

  LineEquation get_line_equation() const {
    return get_line_equation(x, dx, y, dy);
  }
};




Output day_2023_24(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<TwinkleStar> stars;

  for (const std::string & line : lines) {
    std::vector<i64> vals = bj::extract_signed_i64_from_line(line);
    stars.emplace_back(
      TwinkleStar {
        vals[0], vals[1], vals[2], vals[3], vals[4], vals.at(5)
      }
    );
  }

  double min_bound = dei.part_a_extra_param == 0 ? 7. : 200000000000000.;
  double max_bound = dei.part_a_extra_param == 0 ? 27. : 400000000000000.;

  const auto check_boundaries = [&](double val) {
    return min_bound <= val && val <= max_bound;
  };

  i64 part_a = 0;
  for (size_t i = 0; i != stars.size(); ++i) {
    for (size_t j = i + 1; j < stars.size(); ++j) {
      LineEquation a = stars[i].get_line_equation();
      LineEquation b = stars[j].get_line_equation();

      const auto intersect = a.intersect(b);

      if (intersect
        && stars[i].has(intersect->first, intersect->second)
        && stars[j].has(intersect->first, intersect->second)
        && check_boundaries(intersect->first)
        && check_boundaries(intersect->second)
      ) {
        ++part_a;
      }
    }
  }

  return Output(part_a, "Run js/2023_24.py");
}
