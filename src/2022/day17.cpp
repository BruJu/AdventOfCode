#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <cmath>
#include <map>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <set>
#include <list>
#include <array>
#include <optional>
#include <bitset>
#include <array>
#include <span>

// https://adventofcode.com/2022/day/17

static constexpr size_t LastLines = 5;

struct Grid {
  std::set<bj::Position> m_stabilized_positions;
  std::vector<bj::Position> current_falling_position;
  long long int number_of_rocks = 0;

  void on_descend();
  void jet_stream(int dx) { try_move(dx, 0); }

  int get_highest_y() const;

  void draw(int max_y);

  int get_height_score(int y) const {
    int score = 0;

    for (int x = 0; x < 7; ++x) {
      if (m_stabilized_positions.contains(bj::Position{ x, y })) {
        score += 1 << (x);
      }
    }

    return score;
  }

  void add_falling_rock(const char * rock);

  [[nodiscard]] std::array<int, LastLines> get_last_lines() const {
    std::array<int, LastLines> l;

    const int height = get_highest_y();
    for (int i = 0; i != LastLines; ++i) {
      l[i] = get_height_score(height - 1 - i);
    }

    return l;
  }

private:
  bool try_move(int dx, int dy);
  void stabilize();
};

void Grid::draw(int max_y) {
  for (int y = max_y; y >= -1; --y) {
    if (y != -1) std::cout << '|'; else std::cout << '+';
    for (int x = 0; x < 7; ++x) {
      bj::Position pos { x, y };

      if (y == -1) std::cout << '-';
      else if (m_stabilized_positions.contains(pos)) {
        std::cout << '#';
      } else if (auto it = std::find(current_falling_position.begin(), current_falling_position.end(), pos); it != current_falling_position.end()) {
        std::cout << 'v';
      } else {
        std::cout << ' ';
      }
    }

    if (y != -1) std::cout << '|'; else std::cout << '+';
    std::cout << "\n";
  }
}

bool Grid::try_move(int dx, int dy) {
  const auto can_move = [&](int dx, int dy) {
    for (const bj::Position & pos : current_falling_position) {
      bj::Position delta = pos;
      delta.x += dx;
      delta.y += dy;

      if (delta.x < 0) return false;
      if (delta.x >= 7) return false;
      if (delta.y < 0) return false;
      if (m_stabilized_positions.contains(delta)) return false;
    }

    return true;
  };

  const auto move = [&](int dx, int dy) {
    for (bj::Position & pos : current_falling_position) {
      pos.x += dx;
      pos.y += dy;
    }
  };

  if (can_move(dx, dy)) {
    move(dx, dy);
    return true;
  } else {
    return false;
  }
}

int Grid::get_highest_y() const {
  int highest = 0;

  for (const bj::Position & position : m_stabilized_positions) {
    if (position.y + 1 > highest) {
      highest = position.y + 1;
    }
  }

  return highest;
}

void Grid::on_descend() {
  if (try_move(0, -1)) {
    // noop
  } else {
    stabilize();
  }
}

void Grid::stabilize() {
  for (const bj::Position & pos : current_falling_position) {
    m_stabilized_positions.emplace(pos);
  }

  current_falling_position.clear();
  ++number_of_rocks;
}

struct JetPattern {
  std::string pattern;
  size_t i = 0;

  [[nodiscard]] int next_dx() {
    const char p = pattern[i];
    i = (i + 1) % pattern.size();
    if (p == '<') return -1;
    else if (p == '>') return 1;
    else throw std::runtime_error("bad jet");
  }
};

struct RockAdder {
  static constexpr std::array<const char *, 5> falling_rocks = {
    "####",
    ".#. ### .#.",
    "..# ..# ###",
    "# # # #",
    "## ##"
  };

  size_t next_rock_id = 0;

  [[nodiscard]] const char * get_next_rock() {
    const char * rock = falling_rocks[next_rock_id];
    next_rock_id = (next_rock_id + 1) % falling_rocks.size();
    return rock;
  }
};

void Grid::add_falling_rock(const char * rock_type) {
  int x = 2;
  int y = get_highest_y() + 3;

  while (*rock_type) {
    if (*rock_type == ' ') {
      x = 2;
      
      for (bj::Position & pos : current_falling_position) {
        ++pos.y;
      }
    } else if (*rock_type == '#') {
      current_falling_position.emplace_back(bj::Position{ x, y });
      ++x;
    } else {
      ++x;
    }
    ++rock_type;
  }
}

struct CycleDetector {
  size_t pattern_pos;
  size_t rock_pos;
  std::array<int, LastLines> last_lines;

  bool operator==(const CycleDetector & other) const {
    return pattern_pos == other.pattern_pos && rock_pos == other.rock_pos && last_lines == other.last_lines;
  }
};


class FallingCavern {
private:
  Grid present_rocks;
  JetPattern jet_stream;
  RockAdder future_rocks;

public:
  explicit FallingCavern(std::string jet_pattern) : jet_stream{ jet_pattern } {}

  void tick() {
    if (present_rocks.current_falling_position.size() == 0) {
      present_rocks.add_falling_rock(future_rocks.get_next_rock());
    }

    present_rocks.jet_stream(jet_stream.next_dx());
    present_rocks.on_descend();
  }

  [[nodiscard]] CycleDetector get_state_for_cycle_detection() const {
    return CycleDetector { jet_stream.i, future_rocks.next_rock_id, present_rocks.get_last_lines() };
  };

  [[nodiscard]] long long int number_of_rocks() const noexcept { return present_rocks.number_of_rocks; }

  [[nodiscard]] long long int get_highest_y() const noexcept { return present_rocks.get_highest_y(); }

  void add_fake_rocks(long long int nb) { present_rocks.number_of_rocks += nb; }
};



Output day_2022_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // ==== PART A ====
  FallingCavern cavern(lines[0]);

  while (cavern.number_of_rocks() != 2022) {
    cavern.tick();
  }

  const auto ref_current_height = cavern.get_highest_y();

  // ==== PART B ====

  // Grid state to find a cycle
  const CycleDetector ref_state = cavern.get_state_for_cycle_detection();

  while (true) {
    cavern.tick();

    const CycleDetector now_state = cavern.get_state_for_cycle_detection();
    if (now_state == ref_state) {
      break;
    }
  }

  // Let's pretend it's a cycle
  const auto after_cycle_height = cavern.get_highest_y();
  
  const auto height_diff = after_cycle_height - ref_current_height;
  const auto rock_diff = cavern.number_of_rocks() - 2022;

  // Make the cycles fall
  static constexpr long long int part_b_rocks = 1000000000000;
  const auto n = (part_b_rocks - cavern.number_of_rocks()) / rock_diff;
  cavern.add_fake_rocks(rock_diff * n);

  // Finish the simulation
  while (cavern.number_of_rocks() != part_b_rocks) {
    cavern.tick();
  }

  // Compute added height by simulation
  long long int height_b = cavern.get_highest_y();
  long long int computed_height = n * height_diff;
  height_b += computed_height;

  // ==== DONE ====
  return Output(ref_current_height, height_b);
}
