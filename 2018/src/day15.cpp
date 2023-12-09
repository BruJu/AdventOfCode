#include "../../common/advent_of_code.hpp"
#include <span>
#include "../../util/position.hpp"
#include <algorithm>
#include <variant>

// https://adventofcode.com/2018/day/15

// Does not work

struct Empty {};

struct Wall {};

struct Entity {
  enum class Race { Golbin, Elve };
  int id;
  Race race;
  int hit_points;
};

using X = std::variant<Empty, Wall, Entity>;

struct Game {
  std::vector<std::vector<X>> m_grid;

  explicit Game(const std::vector<std::string> & lines);
  [[nodiscard]] bool finished() const;
  void round();
};

Game::Game(const std::vector<std::string> & lines) {
  int entity_id = 0;
  for (int y = 0; std::cmp_less(y, lines.size()); ++y) {
    m_grid.emplace_back();
    for (int x = 0; std::cmp_less(y, lines[y].size()); ++x) {
      const char c = lines[y][x];

      if (c == '#') {
        m_grid.back().emplace_back(Wall{});
      } else if (c == 'G') {
        m_grid.emplace_back(Entity{ ++entity_id, Entity::Race::Golbin });
      } else if (c == 'E') {
        m_grid.emplace_back(Entity{ ++entity_id, Entity::Race::Elve });
      } else {
        m_grid.emplace_back(Empty{});
      }
    }
  }
}

bool Game::finished() const {
  bool hasElve = false;
  bool hasGoblin = false;

  for (const auto & row : m_grid) {
    for (const auto & slot : row) {
      const Entity * entity = std::get_if<Entity>(&slot);
      if (!entity) continue;

      if (entity->race == Entity::Race::Golbin) hasGoblin = true;
      if (entity->race == Entity::Race::Elve) hasElve = true;
    }
  }

  return !(hasElve && hasGoblin);
}

#include <set>

bj::Direction goTo(const Game & game, bj::Position pos, Entity::Race race) {
  struct Path {
    int steps;
    bj::Direction first_direction;

    Path(bj::Direction first_direction) : steps(1), first_direction(first_direction) {}
    Path(Path previous, bj::Direction direction) : steps(previous.steps + 1), first_direction(previous.first_direction) {}

    static bool comp_dir(bj::Direction lhs, bj::Direction rhs) {
      std::array<bj::Direction, 4> dirs{ bj::Direction::Top, bj::Direction::Left, bj::Direction::Right, bj::Direction::Down };

      auto lhs_it = std::find(dirs.begin(), dirs.end(), lhs);
      auto rhs_it = std::find(dirs.begin(), dirs.end(), rhs);

      return lhs_it < rhs_it;
    }

    bool operator<(const Path & other) const {
      if (steps < other.steps) return true;
      if (steps > other.steps) return false;
      if (comp_dir(first_direction, other.first_direction)) return true;
      return false;
    }
  };

  std::set<Path> valid;
  std::set<Path> left;



}

void Game::round() {
  std::vector<bj::Position> entity_positions;

  for (int row = 0; row < m_grid.size(); ++row) {
    for (int col = 0; col < m_grid[0].size(); ++col) {
      const Entity * entity = std::get_if<Entity>(&m_grid[row][col]);
      if (entity) {
        entity_positions.emplace_back(bj::Position{ row, col });
      }
    }
  }

  size_t i = 0;
  while (i < entity_positions.size()) {


  }


}


Output day_2018_15(const std::vector<std::string> & lines, const DayExtraInfo &) {
  Game game(lines);

  while (!game.finished()) {
    game.round();
  }

  return Output(0, 0);
}
