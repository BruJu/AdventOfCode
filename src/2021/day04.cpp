#include "../advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <array>

/**
 * A bingo grid
 */
struct Grid {
  /** Numbers of the grid */
  std::array<int, 5*5> m_numbers_on_grid;
  /** Values that were drawned */
  std::array<bool, 5*5> m_ticked_values;

  /** Build the grid from a vector of 25 integers that are the initial values */
  explicit Grid(const std::vector<int> & values) {
    if (values.size() != 25) {
      std::cerr << "Read a grid of size != 25, = " << values.size() << '\n';
      m_numbers_on_grid.fill(0);
    } else {
      for (size_t i = 0; i != 25; ++i) {
        m_numbers_on_grid[i] = values[i];
      }
    }

    m_ticked_values.fill(false);
  }

  /** Hello hello */
  bool tick(int value) {
    const auto pos_in_grid = std::find(m_numbers_on_grid.begin(), m_numbers_on_grid.end(), value);

    if (pos_in_grid == m_numbers_on_grid.end()) return false;

    const auto tick_at = m_ticked_values.begin() + (pos_in_grid - m_numbers_on_grid.begin());
    (*tick_at) = true;
    return true;
  }

  /** can you here me? */
  [[nodiscard]] bool is_winner() const {
    for (size_t i = 0; i != 5; ++i) {
      bool col_win = true;
      bool row_win = true;
      for (size_t j = 0; j != 5; ++j) {
        if (!m_ticked_values[i * 5 + j]) col_win = false;
        if (!m_ticked_values[j * 5 + i]) row_win = false;
      }

      if (col_win || row_win) return true;
    }

    return false;
  }

  /** Sum all the numbers that were not yet drawned */
  [[nodiscard]] int sum_of_unticked() const {
    int sum = 0;
    for (size_t i = 0; i != 25; ++i) {
      if (!m_ticked_values[i]) {
        sum += m_numbers_on_grid[i];
      }
    }
    return sum;
  }
};

/**
 * Converts a string to a list of integers
 */
std::vector<int> string_to_ints(const std::string & s, char separator) {
  std::vector<int> result;
  StringSplitter splitter(std::string_view(s), separator);

  while (splitter) {
    const auto here = splitter();
    if (here == "") continue;
    result.push_back(std::stol(here));
  }

  return result;
}

Output day_2021_04(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // Parsing the drawn numbers
  std::vector<int> bingo = string_to_ints(lines[0], ',');

  // Parsing the grids
  const std::vector<std::string> grid_lines(lines.begin() + 2, lines.end());

  std::vector<Grid> grids = lines_transform::group<Grid, std::vector<int>>(
    grid_lines,
    [](const std::string & s) { return string_to_ints(s, ' '); },
    [](std::vector<int> lhs, const std::vector<int> & rhs) {
      for (const auto v : rhs) {
        lhs.push_back(v);
      }
      return lhs;
    },
    [](std::vector<int> values) { return Grid(values); }
  );

  // Let's play
  std::optional<std::pair<Grid, int>> first_to_win;
  std::optional<std::pair<Grid, int>> last_to_win;

  for (const int bingo_value : bingo) {
    for (auto it = grids.begin(); it != grids.end();) {
      auto & grid = *it;
      const auto is_here = grid.tick(bingo_value);
      if (is_here && grid.is_winner()) {
        const auto pair = std::pair(grid, bingo_value);
        if (!first_to_win) first_to_win = pair;
        last_to_win = pair;

        it = grids.erase(it);
      } else {
        ++it;
      }
    }
  }

  // Formatting answers
  int part_a_answer = first_to_win->first.sum_of_unticked() * first_to_win->second;
  int part_b_answer = last_to_win->first.sum_of_unticked() * last_to_win->second;

  return Output(part_a_answer, part_b_answer);
}
