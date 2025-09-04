#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>

// https://adventofcode.com/2024/day/25

using i64 = std::int64_t;
namespace {

struct Key;
  
struct Lock {
private:
  std::array<int, 5> _components;

public:
  friend Key;

  explicit Lock(std::array<int, 5> components) : _components(components) {

  }

  friend std::ostream & operator<<(std::ostream & stream, const Lock & self) {
    return stream << "Lock("
      << self._components[0] << ','
      << self._components[1] << ','
      << self._components[2] << ','
      << self._components[3] << ','
      << self._components[4] << ")";
  }
};

struct Key {
private:
  std::array<int, 5> _components;

public:

  explicit Key(std::array<int, 5> components) : _components(components) {
  }

  bool can_fit_in(const Lock & lock) const {
    for (size_t i = 0; i != 5; ++i) {
      if (_components[i] + lock._components[i] > 7) {
        return false;
      }
    }

    return true;
  }
  
  friend std::ostream & operator<<(std::ostream & stream, const Key & self) {
    return stream << "Key("
      << self._components[0] << ','
      << self._components[1] << ','
      << self._components[2] << ','
      << self._components[3] << ','
      << self._components[4] << ")";
  }
};

void read_lines(const std::vector<std::string> & lines, std::vector<Lock> & locks, std::vector<Key> & keys) {

  auto it = lines.begin();

  while (true) {
    while (it != lines.end() && it->empty()) {
      ++it;
    }

    if (it == lines.end()) break;
    
    std::array<int, 5> components;
    components.fill(0);
    bool isLock = (*it)[0] == '#';

    while (it != lines.end() && !it->empty()) {
      for (size_t i = 0; i != 5; ++i) {
        if ( (*it)[i] == '#' ) {
          components[i] += 1;
        }
      }

      ++it;
    }

    if (isLock) {
      locks.emplace_back(components);
    } else {
      keys.emplace_back(components);
    }
  }
}

}

Output day_2024_25(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  std::vector<Lock> locks;
  std::vector<Key> keys;

  read_lines(lines, locks, keys);

  i64 answer = 0;
  for (const Lock & lock : locks) {
    for (const Key & key : keys) {
      if (key.can_fit_in(lock)) {
        ++answer;
      }
    }
  }


  return Output(answer, 0);
}
