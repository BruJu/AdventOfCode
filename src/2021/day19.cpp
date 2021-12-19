#include "../advent_of_code.hpp"
#include <vector>
#include <numeric>
#include <array>
#include <algorithm>
#include <set>
#include <map>
#include <optional>
#include <stack>
#include <cstring>
#include <ostream>

struct Position : std::array<long long int, 3> {
  long long int & x() { return (*this)[0]; }
  long long int & y() { return (*this)[1]; }
  long long int & z() { return (*this)[2]; }

  [[nodiscard]] long long int x() const { return (*this)[0]; }
  [[nodiscard]] long long int y() const { return (*this)[1]; }
  [[nodiscard]] long long int z() const { return (*this)[2]; }

  Position(long long int x, long long int y, long long int z) {
    this->x() = x;
    this->y() = y;
    this->z() = z;
  }

  explicit Position(const std::string & s) {
    const size_t v1 = s.find(',');
    const size_t v2 = s.find(',', v1 + 1);

    x() = std::stoll(s.substr(0, v1));
    y() = std::stoll(s.substr(v1 + 1, v2));
    z() = std::stoll(s.substr(v2 + 1));
  }

  friend std::ostream & operator<<(std::ostream & stream, const Position & self) {
    return stream << self.x() << ',' << self.y() << ',' << self.z();
  }

  void turn_right() {
    const auto p = x();
    x() = -y();
    y() = p;
  }

  Position turn_right(int nb) const {
    auto copy = *this;
    for (int i = 0; i != nb; ++i) copy.turn_right();
    return copy;
  }

  void push() {
    const auto p = x();
    x() = -z();
    z() = p;
  }

  Position push(int nb) const {
    auto copy = *this;
    for (int i = 0; i != nb; ++i) copy.push();
    return copy;
  }

  void angle() {
    const auto p = y();
    y() = z();
    z() = -p;
  }

  Position angle(int nb) const {
    auto copy = *this;
    for (int i = 0; i != nb; ++i) copy.angle();
    return copy;
  }

  Position & operator+=(Position other) {
    x() += other.x();
    y() += other.y();
    z() += other.z();
    return *this;
  }

  Position & operator-=(Position other) {
    x() -= other.x();
    y() -= other.y();
    z() -= other.z();
    return *this;
  }

  Position operator+(Position other) const {
    Position copy = *this;
    copy += other;
    return copy;
  }

  Position operator-(Position other) const {
    Position copy = *this;
    copy -= other;
    return copy;
  }
};

static long long int manhattan(Position lhs, Position rhs) {
  constexpr auto diff = [](long long int l, long long int r) {
    if (l < r) return r - l;
    else return l - r;
  };

  return diff(lhs.x(), rhs.x())
    + diff(lhs.y(), rhs.y())
    + diff(lhs.z(), rhs.z());
}

struct Scanner {
  int number;
  std::vector<Position> points;

  Scanner(std::vector<std::string>::const_iterator & it, std::vector<std::string>::const_iterator end) {
    number = std::stol((*it).substr(std::strlen("--- scanner ")));
    ++it;

    while (it != end && *it != "") {
      points.emplace_back(*it);
      ++it;
    }
  }

  friend std::ostream & operator<<(std::ostream & stream, const Scanner & self) {
    stream << "-- scanner " << self.number << " ---\n";

    for (const auto & point : self.points) {
      stream << point << '\n';
    }

    return stream;
  }

  void push() {
    for (auto & point : points) {
      point.push();
    }
  }

  void turn() {
    for (auto & point : points) {
      point.turn_right();
    }
  }


  void angle() {
    for (auto & point : points) {
      point.angle();
    }
  }

  


};


struct ValidIntegrator {
  int nb_angle;
  int nb_turn;
  int nb_push;
  Position offset;
};

struct KnownBeacons {
  std::set<Position> scanners;
  std::set<Position> beacons;
  size_t cap = 12;
  
  KnownBeacons(const Scanner & scanner) {
    scanners.emplace(Position(0, 0, 0));

    for (const auto & beacon : scanner.points) {
      beacons.emplace(beacon);
    }
  }

  bool integrate(const Scanner & scanner) {
    const auto integrator = get_integrator(scanner);

    if (!integrator) return false;

    const auto [nb_angle, nb_turn, nb_push, offset] = *integrator;

    scanners.emplace(offset);

    for (const auto & beacon : scanner.points) {
      beacons.emplace(beacon.angle(nb_angle).turn_right(nb_turn).push(nb_push) - offset);
    }

    {
      const auto old = cap;
      cap = scanner.points.size();
      const auto integrator = get_integrator(scanner);
      if (!integrator) std::cerr << ":(\n";
      cap = old;
    }
    

    return true;
  }


  std::optional<ValidIntegrator> get_integrator(Scanner scanner) const {
    for (int nb_angle = 0; nb_angle != 4; ++nb_angle, scanner.angle()) {
      for (int nb_turn = 0; nb_turn != 4; ++nb_turn, scanner.turn()) {
        for (int nb_push = 0; nb_push != 4; ++nb_push, scanner.push()) {
          const auto offset = get_integrator_turned(scanner);
          if (offset) return ValidIntegrator{ nb_angle, nb_turn, nb_push, offset.value() };
        }
      }
    }

    return std::nullopt;
  }

  std::optional<Position> get_integrator_turned(const Scanner & scanner) const {
    for (const auto & beacon : beacons) {
      for (const auto & point : scanner.points) {
        // we assume that beacon == point
        const auto offset = point - beacon; // How much do we need to remove from point to go to beacon ?
        const auto bonk = collide(scanner.points, offset);
        if (bonk) return offset;
      }
    }

    return std::nullopt;
  }

  bool collide(const std::vector<Position> & points, Position offset) const {
    size_t score = 0;
    for (const auto & point : points) {
      const auto pt = point - offset;
      if (beacons.find(pt) != beacons.end()) {
        ++score;
      }
    }

    return score >= cap;
  }
};


Output day_2021_19(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<Scanner> scanners;

  auto it = lines.begin();
  while (it != lines.end()) {
    if (*it == "") { ++it; continue; }

    scanners.emplace_back(it, lines.end());
  }

//  // Display input
//  for (const auto & scanner : scanners) {
//    std::cout << scanner << '\n';
//  }

  const auto original_nb = scanners.size();

  KnownBeacons beacons(scanners[0]);
  if (dei.part_a_extra_param != 0) {
    beacons.cap = dei.part_a_extra_param;
  } else {
    beacons.cap = 12;
  }

  scanners.erase(scanners.begin());

  bool stable = false;
  while (scanners.size() != 0 && !stable) {
    stable = true;

    auto it = scanners.begin();
    while (it != scanners.end()) {
      const auto ok = beacons.integrate(*it);
      if (ok) {
        it = scanners.erase(it);
        stable = false;
      } else {
        ++it;
      }
    }
  }

  if (scanners.size() != 0) {
    std::cout << "bad " << scanners.size() << " / " << original_nb << "\n";
  }

//  // Display scanners location
//  for (const auto & sc : beacons.scanners) {
//    std::cout << sc << '\n';
//  }

  long long int max_distance = 0;

  for (auto i = beacons.scanners.begin(); i != beacons.scanners.end(); ++i) {
    for (auto j = i; j != beacons.scanners.end(); ++j) {
      if (j == i) continue;
      const auto dist = manhattan(*i, *j);
      if (max_distance < dist) {
        max_distance = dist;
      }
    }
  }

  return Output(beacons.beacons.size(), max_distance);
}
