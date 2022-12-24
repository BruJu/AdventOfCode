#include "../advent_of_code.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include "../util/position.hpp"
#include <set>
#include <span>
#include <array>
#include <bitset>

// https://adventofcode.com/2022/day/24

// A blizzard situation
struct Blizzards {
  static constexpr size_t dir_left = 0;
  static constexpr size_t dir_right = 1;
  static constexpr size_t dir_up = 2;
  static constexpr size_t dir_down = 3;

  explicit Blizzards(const std::vector<std::string> & lines);
  [[nodiscard]] Blizzards next() const;
  [[nodiscard]] bool is_blocked(bj::Position position) const;

  [[nodiscard]] int width() const { return static_cast<int>(blizzards[0].size()); }
  [[nodiscard]] int height() const noexcept { return static_cast<int>(blizzards.size()); }

  void print() const;

  [[nodiscard]] bool operator==(const Blizzards & other) const {
    return blizzards == other.blizzards;
  }

private:
  Blizzards() = default;
  [[nodiscard]] static std::bitset<4> to_bitset(char c);

  std::vector<std::vector<std::bitset<4>>> blizzards;
};

Blizzards::Blizzards(const std::vector<std::string> & lines) {
  for (const std::string & line : lines) {
    blizzards.emplace_back();
    for (const char c : line) {
      if (c == '#') continue;
      blizzards.back().emplace_back(to_bitset(c));
    }
  }

  blizzards.erase(blizzards.begin());
  blizzards.erase(blizzards.end() - 1);
}

std::bitset<4> Blizzards::to_bitset(const char c) {
  std::bitset<4> retval{ 0 };
  switch (c) {
    case '<': retval[dir_left]  = true; break;
    case '>': retval[dir_right] = true; break;
    case 'v': retval[dir_down]  = true; break;
    case '^': retval[dir_up]    = true; break;
  }
  return retval;
}

Blizzards Blizzards::next() const {
  Blizzards retval;

  for (const auto & line : blizzards) {
    retval.blizzards.emplace_back();
    retval.blizzards.back().resize(line.size(), 0);
  }

  const int width_ = width();
  const int height_ = height();

  for (int y = 0; y != height_; ++y) {
    for (int x = 0; x != width_; ++x) {
      std::bitset<4> & bitset = retval.blizzards[y][x];

      const int xleft = x == 0 ? width_  - 1 : x - 1;
      const int ytop  = y == 0 ? height_ - 1 : y - 1;
      const int xright  = x + 1 == width_  ? 0 : x + 1;
      const int ybottom = y + 1 == height_ ? 0 : y + 1;

      if (blizzards[ytop][x].test(dir_down)) bitset.set(dir_down);
      if (blizzards[y][xleft].test(dir_right)) bitset.set(dir_right);
      if (blizzards[ybottom][x].test(dir_up)) bitset.set(dir_up);
      if (blizzards[y][xright].test(dir_left)) bitset.set(dir_left);
    }
  }

  return retval;
}

bool Blizzards::is_blocked(bj::Position position) const {
  return blizzards[position.y][position.x].any();
}

void Blizzards::print() const {
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
      const size_t size = blizzards[y][x].count();

      if (size == 0) std::cout << '.';
      else if (size > 1) std::cout << (char)('0' + size);
      else {
        if (blizzards[y][x][dir_down]) std::cout << 'v';
        else if (blizzards[y][x][dir_left]) std::cout << '<';
        else if (blizzards[y][x][dir_right]) std::cout << '>';
        else if (blizzards[y][x][dir_up]) std::cout << '^';
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

  State(bj::Position where, size_t blizzard_id) : where(where), blizzard_id(blizzard_id) {}

  bool operator<(const State & state) const {
    if (where < state.where) return true;
    if (state.where < where) return false;
    return blizzard_id < state.blizzard_id;
  }
};

struct Blizzardss {
  std::vector<Blizzards> all_setups;
  int width;
  int height;

  explicit Blizzardss(const std::vector<std::string> & lines);

  [[nodiscard]] int distance_to_end(State state) const;
};

Blizzardss::Blizzardss(const std::vector<std::string> & lines) {
  Blizzards current(lines);

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

int Blizzardss::distance_to_end(const State state) const {
  return (width - 1 - state.where.x) + (height - state.where.y);
}

// 
#include <queue>

struct StateAndWalk {
  State state;
  size_t walk;

  StateAndWalk(State state, size_t walk) : state(state), walk(walk) {}
};

struct BFS {
  const Blizzardss * blizzardss;
  std::map<State, size_t> visited;
  std::queue<StateAndWalk> to_visit;

  std::optional<size_t> best = std::nullopt;

  explicit BFS(const Blizzardss & blizzards);

  std::optional<size_t> visit_states();
  void visit_state();

  void try_add(bj::Position where, size_t blizzard_setup, size_t walked);

  [[nodiscard]] static bool is_legal_position(const bj::Position position, const Blizzards & blizzards);
};

BFS::BFS(const Blizzardss & blizzards) : blizzardss(&blizzards) {
  State state(bj::Position{ 0, -1 }, 0);
  to_visit.emplace(StateAndWalk(state, 0));
  visited.emplace(state, 0);
}

bool BFS::is_legal_position(const bj::Position position, const Blizzards & blizzards) {
  if (position.y < -1 || position.y > blizzards.height()) return false;
  if (position.x <= -1 || position.x >= blizzards.width()) return false;

  if (position.y == -1) {
    return position.x == 0;
  } else if (position.y == blizzards.height()) {
    return position.x + 1 == blizzards.width();
  } else {
    return !blizzards.is_blocked(position);
  }
}

std::optional<size_t> BFS::visit_states() {
  while (!to_visit.empty()) {
    visit_state();
  }

  return best;
}

void BFS::visit_state() {
  const auto [state, walked] = to_visit.front();
  to_visit.pop();

  if (best && blizzardss->distance_to_end(state) + walked >= *best) return;

  const size_t next_setup_id = (state.blizzard_id + 1) % blizzardss->all_setups.size();

  std::array<bj::Position, 5> nexts;
  nexts[0] = state.where; nexts[0].y += 1;
  nexts[1] = state.where; nexts[1].x += 1;
  nexts[2] = state.where;
  nexts[3] = state.where; nexts[3].y -= 1;
  nexts[4] = state.where; nexts[4].x -= 1;
  
  for (const auto & next : nexts) {
    try_add(next, next_setup_id, walked + 1);
  }
}

void BFS::try_add(bj::Position where, size_t blizzard_setup, size_t walked) {
  const Blizzards & blizzards = blizzardss->all_setups[blizzard_setup];

  if (!is_legal_position(where, blizzards)) return;

  State state{ where, blizzard_setup };

  const auto in_visited = visited.find(state);
  if (in_visited != visited.end() && in_visited->second <= walked) return;

  visited[state] = walked;

  if (where.y == blizzards.height()) {
    best = walked;
  } else {
    to_visit.emplace(state, walked);
  }
}

Output day_2022_24(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Blizzardss bs(lines);
  std::cout << bs.all_setups.size() << "\n";

  BFS bfs(bs);

  const auto answer = bfs.visit_states();

  std::cout << bfs.visited.size() << "\n";

  return Output(answer.value_or(-1), 0);
}
