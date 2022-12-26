#include "../advent_of_code.hpp"
#include <algorithm>
#include <numeric>
#include <regex>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <map>

// https://adventofcode.com/2017/day/20

struct XYZ {
  std::array<std::int64_t, 3> raw;

  XYZ(std::int64_t x, std::int64_t y, std::int64_t z) {
    raw[0] = x;
    raw[1] = y;
    raw[2] = z;
  }

  [[nodiscard]] std::int64_t manhattan() const {
    std::int64_t abs = 0;
    for (size_t i = 0; i != 3; ++i) {
      abs += std::abs(raw[i]);
    }
    return abs;
  }

  [[nodiscard]] std::int64_t operator[](size_t i) const { return raw[i]; }

  XYZ & operator+=(const XYZ & other) {
    for (size_t i = 0; i != 3; ++i) {
      raw[i] += other.raw[i];
    }
    return *this;
  }
};

struct Particle {
  XYZ position;
  XYZ speed;
  XYZ acceleration;

  bool destroyed = false;

  std::optional<std::int64_t> get_time_of_collision(const Particle & other) const;

  static std::int64_t get_position(std::int64_t time, std::int64_t pos, std::int64_t speed, std::int64_t acceleration);

  std::vector<std::int64_t> get_time_of_collision(const Particle & other, size_t axis) const;
};


std::int64_t Particle::get_position(std::int64_t time, std::int64_t pos, std::int64_t speed, std::int64_t acceleration) {
  return pos + time * speed + acceleration * time * (time + 1) / 2;
}


std::vector<std::int64_t> Particle::get_time_of_collision(const Particle & other, size_t axis) const {
  // p(t) = p + v*t + a*t(t+1)/2
  //      = p + v*t + a*(t²+t) / 2
  //      = p + v*t + at²/2 + at/2
  //      = p + t*(v + a/2) + t²a/2
  // Equivalent to solving a ax² + bx + c = 0 formula

  // On the x axis
  const double a = (acceleration[axis] - other.acceleration[axis]) / 2.0;
  const double b = speed[axis] - other.speed[axis] + (acceleration[axis] - other.acceleration[axis]) / 2.0;
  const double c = position[axis] - other.position[axis];

  constexpr auto to_integer = [](double value) -> std::optional<std::int64_t> {
    double throwaway;
    const auto decimal = std::modf(value, &throwaway);
    if (decimal != 0.0) return std::nullopt;
    if (value <= -0.0) return std::nullopt;
    return static_cast<std::int64_t>(throwaway);
  };

  if (a == 0.0 || a == -0.0) {
    // bx + c = 0
    // x = -c / b

    if (b == 0.0 || b == -0.0) {
      if (c == 0) {
        return { 0, 1, 2 }; // All
      } else {
        return {}; // None
      }
    }

    const auto val = to_integer(- c / b);
    if (!val) return {};

    return { val.value() };
  }

  const double delta = b * b - 4 * a * c;
  if (delta < -0.0) return {};

  std::vector<double> roots;
  roots.emplace_back((-b - std::sqrt(delta)) / (2.0 * a));
  if (delta != 0.0 && delta != -0.0) {
    roots.emplace_back((-b + std::sqrt(delta)) / (2.0 * a));
  }

  std::sort(roots.begin(), roots.end());

  std::vector<std::int64_t> retval;

  for (double root : roots) {
    if (const auto as_int = to_integer(root)) {
      retval.emplace_back(*as_int);
    }
  }

  return retval;
}

std::optional<std::int64_t> Particle::get_time_of_collision(const Particle & other) const {
  auto x = get_time_of_collision(other, 0);
  auto y = get_time_of_collision(other, 1);
  auto z = get_time_of_collision(other, 2);

  if (x.size() == 0) return std::nullopt;
  if (y.size() == 0) return std::nullopt;
  if (z.size() == 0) return std::nullopt;

  std::vector<std::int64_t> to_check;

  if (x.size() == 3) {
    if (y.size() != 3) {
      x = y;
    } else if (z.size() != 3) {
      x = z;
    } else {
      return 0;
    }
  }

  for (const std::int64_t x_root : x) {
    if (y.size() == 3 || y.end() != std::find(y.begin(), y.end(), x_root)) {
    if (z.size() == 3 || z.end() != std::find(z.begin(), z.end(), x_root)) {
      to_check.emplace_back(x_root);
    }
    }
  }

  if (to_check.empty()) return std::nullopt;

  std::sort(to_check.begin(), to_check.end());
  return to_check[0];
}

Output day_2017_20(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::regex regex { R"(p=<\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)>, v=<\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)>, a=<\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)>)" };
  std::smatch smatch;

  std::vector<Particle> particles;
  for (const std::string & line : lines) {
    if (!std::regex_search(line, smatch, regex)) {
      std::cout << "bad line: " << line << "\n";
      throw std::runtime_error("bad regex");
    }

    particles.emplace_back(
      Particle {
        XYZ (std::stoi(smatch[1].str()), std::stoi(smatch[2].str()), std::stoi(smatch[3].str())),
        XYZ (std::stoi(smatch[4].str()), std::stoi(smatch[5].str()), std::stoi(smatch[6].str())),
        XYZ (std::stoi(smatch[7].str()), std::stoi(smatch[8].str()), std::stoi(smatch[9].str()))
      }
    );
  }

  auto min_acceleration = std::min_element(
    particles.begin(), particles.end(),
    [](const Particle & lhs, const Particle & rhs) {
      return lhs.acceleration.manhattan() < rhs.acceleration.manhattan();
    }
  ) - particles.begin();


  std::map<std::int64_t, std::vector<std::pair<size_t, size_t>>> collisions;

  for (size_t i = 0; i != particles.size(); ++i) {
    for (size_t j = i + 1; j < particles.size(); ++j) {
      const Particle & a = particles[i];
      const Particle & b = particles[j];

      const auto collide_at = a.get_time_of_collision(b);
      if (collide_at) {
        collisions[collide_at.value()].emplace_back(std::pair<size_t, size_t>(i, j));
      }
    }
  }

  for (const auto & [time, pairs] : collisions) {
    std::set<size_t> deleted;

    for (const auto [first, second] : pairs) {
      const Particle & a = particles[first];
      const Particle & b = particles[second];

      if (!a.destroyed && !b.destroyed) {
        deleted.emplace(first);
        deleted.emplace(second);
      }
    }

    for (const size_t deleted_item : deleted) {
      particles[deleted_item].destroyed = true;
    }
  }


  size_t alive = 0;
  for (const Particle & particle : particles) {
    if (!particle.destroyed) {
      ++alive;
    }
  }

  return Output(min_acceleration, alive);
}

// < 473