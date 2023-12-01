#include "../../common/advent_of_code.hpp"
#include "../../util/position.hpp"
#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>
#include <vector>

// https://adventofcode.com/2022/day/24

// A blizzard situation
struct Blizzard {
  static constexpr size_t dir_left = 0;
  static constexpr size_t dir_right = 1;
  static constexpr size_t dir_up = 2;
  static constexpr size_t dir_down = 3;

  explicit Blizzard(const std::vector<std::string> & lines);
  [[nodiscard]] Blizzard next() const;

  [[nodiscard]] int width() const { return static_cast<int>(blizzard[0].size()); }
  [[nodiscard]] int height() const noexcept { return static_cast<int>(blizzard.size()); }

  [[nodiscard]] bool operator==(const Blizzard & other) const { return blizzard == other.blizzard; }

  [[nodiscard]] bool is_blocked(bj::Position position) const;
  [[nodiscard]] std::uint8_t compute_step(bj::Position position, std::uint8_t former_step) const;

  void print() const;

private:
  Blizzard() = default;
  [[nodiscard]] static std::bitset<4> to_bitset(char c);

  std::vector<std::vector<std::bitset<4>>> blizzard;
};

Blizzard::Blizzard(const std::vector<std::string> & lines) {
  for (const std::string & line : lines) {
    blizzard.emplace_back();
    for (const char c : line) {
      if (c == '#') continue;
      blizzard.back().emplace_back(to_bitset(c));
    }
  }

  blizzard.erase(blizzard.begin());
  blizzard.erase(blizzard.end() - 1);
}

std::bitset<4> Blizzard::to_bitset(const char c) {
  std::bitset<4> retval{ 0 };
  switch (c) {
    case '<': retval[dir_left]  = true; break;
    case '>': retval[dir_right] = true; break;
    case 'v': retval[dir_down]  = true; break;
    case '^': retval[dir_up]    = true; break;
  }
  return retval;
}

Blizzard Blizzard::next() const {
  Blizzard retval;

  for (const auto & line : blizzard) {
    retval.blizzard.emplace_back();
    retval.blizzard.back().resize(line.size(), 0);
  }

  const int width_ = width();
  const int height_ = height();

  for (int y = 0; y != height_; ++y) {
    for (int x = 0; x != width_; ++x) {
      std::bitset<4> & bitset = retval.blizzard[y][x];

      const int xleft = x == 0 ? width_  - 1 : x - 1;
      const int ytop  = y == 0 ? height_ - 1 : y - 1;
      const int xright  = x + 1 == width_  ? 0 : x + 1;
      const int ybottom = y + 1 == height_ ? 0 : y + 1;

      if (blizzard[ytop][x].test(dir_down)) bitset.set(dir_down);
      if (blizzard[y][xleft].test(dir_right)) bitset.set(dir_right);
      if (blizzard[ybottom][x].test(dir_up)) bitset.set(dir_up);
      if (blizzard[y][xright].test(dir_left)) bitset.set(dir_left);
    }
  }

  return retval;
}

bool Blizzard::is_blocked(bj::Position position) const {
  return blizzard[position.y][position.x].any();
}

std::uint8_t Blizzard::compute_step(const bj::Position position, const std::uint8_t former_step) const {
  if (former_step % 2 == 0) {
    if (position == bj::Position{ width() - 1, height() }) return former_step + 1;
    return former_step;
  } else {
    if (position == bj::Position{ 0, -1 }) return former_step + 1;
    else return former_step;
  }
}

void Blizzard::print() const {
  const int width_ = width();
  const int height_ = height();

  // First line
  std::cout << '#';
  for (int i = 0; i != width_; ++i) {
    if (i == 0) std::cout << '.';
    else std::cout << '#';
  }
  std::cout << '#' << '\n';

  // Other lines
  for (int y = 0; y != height_; ++y) {
    std::cout << '#';

    for (int x = 0; x != width_; ++x) {
      const size_t size = blizzard[y][x].count();

      if (size == 0) std::cout << '.';
      else if (size > 1) std::cout << (char)('0' + size);
      else {
        if (blizzard[y][x][dir_down]) std::cout << 'v';
        else if (blizzard[y][x][dir_left]) std::cout << '<';
        else if (blizzard[y][x][dir_right]) std::cout << '>';
        else if (blizzard[y][x][dir_up]) std::cout << '^';
      }
    }

    std::cout << '#' << '\n';
  }

  
  // Last line
  std::cout << '#';
  for (int i = 0; i != width_; ++i) {
    if (i == width_ - 1) std::cout << '.';
    else std::cout << '#';
  }
  std::cout << '#' << '\n';
}

// All setup precomputations

struct State {
  bj::Position where;
  size_t blizzard_id;
  std::uint8_t step;

  State(bj::Position where, size_t blizzard_id, std::uint8_t step)
    : where(where), blizzard_id(blizzard_id), step(step) {}

  [[nodiscard]] bool operator==(const State & other) const noexcept {
    return where == other.where && blizzard_id == other.blizzard_id && step == other.step;
  }
};

namespace std {
  template <> struct hash<State> {
    std::size_t operator()(const State & state) const noexcept {
      return state.where.x * 100000 + state.where.y * 1000 + state.blizzard_id * 5 + state.step;
    }
  };
}

struct Blizzards {
  std::vector<Blizzard> all_setups;
  int width;
  int height;

  explicit Blizzards(const std::vector<std::string> & lines);

  [[nodiscard]] int distance_to_end(State state) const;
};

Blizzards::Blizzards(const std::vector<std::string> & lines) {
  Blizzard current(lines);

  width = current.width();
  height = current.height();

  const int max = current.width() * current.height();

  for (int i = 0; i != max; ++i) {
    if (!all_setups.empty() && all_setups[0] == current) {
      break;
    }

    all_setups.emplace_back(current);
    current = current.next();
  }
}

int Blizzards::distance_to_end(const State state) const {
  int added = (2 - state.step) * (width + height);

  if (state.step % 2 == 0) {
    return added + (width - 1 - state.where.x) + (height - state.where.y);
  } else {
    return added + (state.where.x + state.where.y + 1);
  }
}

// DFS
struct DFS {
  static std::optional<size_t> run(const Blizzards & blizzards, size_t initial_step);

private:
  const Blizzards * blizzards;
  std::unordered_map<State, size_t> visited;
  std::queue<State> to_visit;

  std::optional<size_t> best = std::nullopt;

  DFS(const Blizzards & blizzards, size_t initial_step);

  void visit_state();

  void try_add(bj::Position where, size_t blizzard_setup, size_t walked, std::uint8_t step);

  [[nodiscard]] static bool is_legal_position(const bj::Position position, const Blizzard & blizzard);
};

std::optional<size_t> DFS::run(const Blizzards & blizzards, size_t initial_step) {
  DFS dfs(blizzards, initial_step);
  
  while (!dfs.to_visit.empty()) {
    dfs.visit_state();
  }

  return dfs.best;
}

DFS::DFS(const Blizzards & blizzards, size_t initial_step) : blizzards(&blizzards) {
  State state(bj::Position{ 0, -1 }, 0, initial_step);
  to_visit.emplace(state);
  visited.emplace(state, 0);
}

bool DFS::is_legal_position(const bj::Position position, const Blizzard & blizzard) {
  if (position.y < -1 || position.y > blizzard.height()) return false;
  if (position.x <= -1 || position.x >= blizzard.width()) return false;

  if (position.y == -1) {
    return position.x == 0;
  } else if (position.y == blizzard.height()) {
    return position.x + 1 == blizzard.width();
  } else {
    return !blizzard.is_blocked(position);
  }
}

void DFS::visit_state() {
  const auto state = to_visit.front();
  const auto walked = visited.find(state)->second;
  to_visit.pop();

  if (best && blizzards->distance_to_end(state) + walked >= *best) return;

  const size_t next_setup_id = (state.blizzard_id + 1) % blizzards->all_setups.size();

  std::array<bj::Position, 5> nexts;
  nexts[0] = state.where; nexts[0].y += 1;
  nexts[1] = state.where; nexts[1].x += 1;
  nexts[2] = state.where;
  nexts[3] = state.where; nexts[3].y -= 1;
  nexts[4] = state.where; nexts[4].x -= 1;
  
  for (const auto & next : nexts) {
    try_add(next, next_setup_id, walked + 1, state.step);
  }
}

void DFS::try_add(bj::Position where, size_t blizzard_setup, size_t walked, std::uint8_t step) {
  const Blizzard & blizzard = blizzards->all_setups[blizzard_setup];

  if (!is_legal_position(where, blizzard)) return;

  State state{ where, blizzard_setup, blizzard.compute_step(where, step) };

  const auto in_visited = visited.find(state);
  if (in_visited != visited.end() && in_visited->second <= walked) return;

  visited[state] = walked;

  if (state.step == 3) {
    best = walked;
  } else {
    to_visit.emplace(state);
  }
}

// Ok
Output day_2022_24(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Blizzards blizzards(lines);

  return Output(
    DFS::run(blizzards, 2).value_or(-1),
    DFS::run(blizzards, 0).value_or(-1)
  );
}
