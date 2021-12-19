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

};


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


};


Output day_2021_19(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<Scanner> scanners;

  auto it = lines.begin();
  while (it != lines.end()) {
    if (*it == "") { ++it; continue; }

    scanners.emplace_back(it, lines.end());
  }

  for (const auto & scanner : scanners) {
    std::cout << scanner << '\n';
  }

  return Output(0, 0);
}
