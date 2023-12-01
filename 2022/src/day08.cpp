#include "../../common/advent_of_code.hpp"
#include <optional>
#include <algorithm>

// https://adventofcode.com/2022/day/8
// A grid of tree heights
//
// 30373
// 25512
// 65332
// 33549
// 35390
//
// (a) How many trees are visible from outside?
// (b) Find the maximum scenic score.
//     scenic score = product of how many trees can be seen for each direction.

// The forest
struct Grid {
public:
  // Status of a slot
  struct SlotStatus {
    bool is_visible_from_outside;
    int scenic_score;
  };

private:
  std::vector<std::vector<int>> m_values;

public:

  // Build the grid
  explicit Grid(const std::vector<std::string> & lines) {
    m_values.reserve(lines.size());
    for (const std::string & line : lines) {
      m_values.emplace_back();
      m_values.back().reserve(line.size());

      for (const char tree : line) {
        m_values.back().emplace_back(tree - '0');
      }
    }
  }

  // Return the tree height at position [i, j], assumes it exists
  [[nodiscard]] int get(size_t i, size_t j) const {
    return m_values[i][j];
  }

  // Compute the slot status of a slot
  [[nodiscard]] SlotStatus compute_slot_status(size_t i, size_t j) const {
    const int my_tree = get(i, j);

    SlotStatus status = SlotStatus { false, 1 };

    using Direction = DirectionIterator::Direction;

    static constexpr std::array<Direction, 4> directions = {
      Direction::Left, Direction::Right,
      Direction::Down, Direction::Up
    };

    for (const auto direction : directions) {
      DirectionIterator it { i, j, direction, this };

      int number_of_seen_trees = 0;

      while (true) {
        const auto next_tree = it.next();
        
        if (!next_tree.has_value()) { // Out of bound
          status.is_visible_from_outside = true;
          break;
        }

        ++number_of_seen_trees;

        if (my_tree <= *next_tree) { // vision blocked
          break;
        }
      }

      status.scenic_score *= number_of_seen_trees;
    }

    return status;
  }

  // Something to iterate on the grid by using a direction.
  // 
  // Implemented as a Rust / JavaScript style iterator because it is easy to implement
  struct DirectionIterator {
    enum class Direction { Left, Up, Down, Right };

    size_t i;
    size_t j;
    Direction direction;
    const Grid * grid;

    std::optional<int> next() {
      if (direction == Direction::Up) {
        if (i == 0) return std::nullopt;
        --i;
      } else if (direction == Direction::Left) {
        if (j == 0) return std::nullopt;
        --j;
      } else if (direction == Direction::Down) {
        if (i >= grid->m_values.size() - 1) return std::nullopt;
        ++i;
      } else if (direction == Direction::Right) {
        if (j >= grid->m_values[0].size() - 1) return std::nullopt;
        ++j;
      }

      return grid->get(i, j);
    }
  };

  // Return the result of AOC 2022-08:
  // - The number of visible trees from the border
  // - The best scenic score
  [[nodiscard]] Output count_visible_and_scenic() const {
    long long int nb_visible_from_outside = 0;
    long long int best_scenic_score = 0;

    for (size_t i = 0; i != m_values.size(); ++i) {
      for (size_t j = 0; j != m_values[0].size(); ++j) {
        const SlotStatus status = compute_slot_status(i, j);
        if (status.is_visible_from_outside) ++nb_visible_from_outside;
        best_scenic_score = std::max<long long int>(best_scenic_score, status.scenic_score);
      }
    }

    return Output(nb_visible_from_outside, best_scenic_score);
  }
};


Output day_2022_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const Grid grid = Grid(lines);
  return grid.count_visible_and_scenic();
}
