#include "../../common/advent_of_code.hpp"
#include <vector>
#include <exception>
#include <span>
#include <optional>
#include <regex>

// https://adventofcode.com/2018/day/11


struct Cache {
  std::int64_t sum = 0;
  int first_line = -1;
  int size = -1;
};


template<typename T, typename I>
struct best_finder {
private:
  I m_score = 0;
  std::vector<T> m_values;

public:
  bool accumulate(const T & value, I score);
  [[nodiscard]] std::span<const T> get_values() const { return m_values; }
  [[nodiscard]] I get_score() const { return m_score; }
};

template<typename T, typename I>
bool best_finder<T, I>::accumulate(const T & value, const I score) {
  if (m_values.empty()) {
    m_score = score;
    m_values.emplace_back(value);
    return true;
  }

  if (m_score == score) {
    m_values.emplace_back(value);
    return true;
  }

  if (m_score < score) {
    m_score = score;
    m_values.clear();
    m_values.emplace_back(value);
    return true;
  }

  return false;
}


static std::int64_t get_rack_power(std::pair<int, int> xy, std::int64_t grid_serial_number) {
  // Find the fuel cell's rack ID, which is its X coordinate plus 10.
  std::int64_t rack_id = xy.first + 10;
  // Begin with a power level of the rack ID times the Y coordinate.
  std::int64_t power_level = rack_id * xy.second;
  // Increase the power level by the value of the grid serial number (your puzzle input).
  power_level += grid_serial_number;
  // Set the power level to itself multiplied by the rack ID.
  power_level = power_level * rack_id;
  // Keep only the hundreds digit of the power level (so 12345 becomes 3; numbers with no hundreds digit become 0).
  std::int64_t hundred = power_level / 100 % 10;
  // Subtract 5 from the power level.
  return hundred - 5;
}

class PowerGrid {
public:
  std::array<std::array<int, 300>, 300> grid;
  explicit PowerGrid(int grid_serial_number);
  [[nodiscard]] std::int64_t get_rack_total_power(int x, int y, int size) const;

  std::int64_t get_total_power(int left, int right, int top, int bottom) const;

  [[nodiscard]] std::int64_t get_rack_total_power(int x, int y, int size, Cache & cache) const;
};


std::int64_t PowerGrid::get_rack_total_power(int x, int y, int size, Cache & cache) const {
  if (cache.first_line == -1) {
    cache.first_line = y;
    cache.size = size;
    cache.sum = get_rack_total_power(x, y, size);
    return cache.sum;
  }

  if (cache.first_line + 1 == y && cache.size == size) {
    cache.sum -= get_total_power(x, x + size - 1, cache.first_line, cache.first_line);
    ++cache.first_line;

    cache.sum += get_total_power(x, x + size - 1, y + size - 1, y + size - 1);

    return cache.sum;
  } else {
    cache.first_line = -1;
    return get_rack_total_power(x, y, size, cache);
  }
}

PowerGrid::PowerGrid(const int grid_serial_number) {
  for (int x = 1; x <= 300; ++x) {
    for (int y = 1; y <= 300; ++y) {
      grid[x - 1][y - 1] = get_rack_power(std::pair(x, y), grid_serial_number);
    }
  }
}

std::int64_t PowerGrid::get_total_power(const int left, const int right, const int top, const int bottom) const {
  if (left < 1 || top < 1 || right > 300 || bottom > 300) {
    std::cout << left << "," << top << "," << right << "," << bottom << "\n";
    throw std::runtime_error("OOB");
  }

  std::int64_t total = 0;

  for (int x = left; x <= right; ++x) {
    for (int y = top; y <= bottom; ++y) {
      total += grid[x - 1][y - 1];
    }
  }

  return total;
}

std::int64_t PowerGrid::get_rack_total_power(const int x, const int y, const int size) const {
  if (x + size > 300 || y + size > 300) return INT64_MIN;

  std::int64_t total_power = 0;

  for (int dx = 0; dx != size; ++dx) {
    for (int dy = 0; dy != size; ++dy) {
      total_power += grid[x + dx - 1][y + dy - 1];
    }
  }

  return total_power;
}


template<size_t N>
std::string array_to_string(std::array<int, N> values) {
  std::string s = std::to_string(values[0]);
  for (size_t i = 1; i < N; ++i) {
    s += ',';
    s += std::to_string(values[i]);
  }
  return s;
}


Output day_2018_11(const std::vector<std::string> &, const DayExtraInfo & dei) {
  const std::int64_t grid_serial_number = dei.part_a_extra_param;
  const PowerGrid grid(grid_serial_number);

  // Part A
  best_finder<std::array<int, 2>, std::int64_t> part_a;
  for (int x = 1; x <= 300 - 3; ++x) {
    Cache cache;
    for (int y = 1; y <= 300 - 3; ++y) {
      part_a.accumulate({ x, y }, grid.get_rack_total_power(x, y, 3, cache));
    }
  }

  // Part B
  best_finder<std::array<int, 3>, std::int64_t> part_b;
  for (int grid_size = 1; grid_size <= 300; ++grid_size) {
    for (int x = 1; x <= 300 - grid_size; ++x) {
      Cache cache;
      for (int y = 1; y <= 300 - grid_size; ++y) {
        const auto current_rack_power = grid.get_rack_total_power(x, y, grid_size, cache);
        part_b.accumulate({ x, y, grid_size }, current_rack_power);
      }
    }
  }

  // Ok
  return Output(
    array_to_string(part_a.get_values()[0]), array_to_string(part_b.get_values()[0])
  );
}
