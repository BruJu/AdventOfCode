#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <array>
#include <bitset>
#include <set>
#include <string>
#include <stack>

// https://adventofcode.com/2017/day/10
// https://adventofcode.com/2017/day/14

// == DAY 10

struct Knothash {
  std::array<std::uint8_t, 16> raw_bytes;

  explicit Knothash(const std::string & str);
  [[nodiscard]] std::string to_string() const;
};

namespace {

std::array<std::uint8_t, 256> get_initial_rope() {
  std::array<std::uint8_t, 256> values;
  for (size_t i = 0; i != 256; ++i) {
    values[i] = static_cast<std::uint8_t>(i);
  }
  return values;
}

struct Knotter {
  size_t current_position = 0;
  size_t skip_size = 0;

  void knot(std::span<std::uint8_t> rope, size_t length) {
    if (length > rope.size()) {
      return;
    }

    reverse(rope, current_position, length);

    current_position = (current_position + length + skip_size) % rope.size();
    ++skip_size;
  }

  static void reverse(std::span<std::uint8_t> rope, size_t current_position, size_t length) {
    for (size_t i = 0; i != length / 2; ++i) {
      size_t left = (current_position + i) % rope.size();
      size_t right = (current_position + length - 1 - i) % rope.size();

      std::swap(rope[left], rope[right]);
    }
  }
};

int part_a(std::string line) {
  const std::vector<std::string> tokens = bj::string_split(line, ",");
  std::vector<size_t> lengths;
  for (const std::string & token : tokens) {
    lengths.emplace_back(std::stoul(token));
  }

  Knotter round_data;
  auto rope = get_initial_rope();

  for (const size_t length : lengths) {
    round_data.knot(rope, length);
  }

  return rope[0] * rope[1];
}

char to_hex_char(int number) {
  if (number >= 0 && number <= 9) return '0' + number;
  else return 'a' + number - 10;
}

}


Knothash::Knothash(const std::string & str) {
  std::vector<size_t> lengths;
  for (const char c : str) {
    lengths.emplace_back(static_cast<size_t>(c));
  }

  for (const int extra : { 17, 31, 73, 47, 23 }) {
    lengths.emplace_back(extra);
  }

  Knotter knotter;
  auto rope = get_initial_rope();

  for (int i = 0; i != 64; ++i) {
    for (const size_t length : lengths) {
      knotter.knot(rope, length);
    }
  }

  for (int i = 0; i != 16; ++i) {
    raw_bytes[i] = 0;
    for (int j = 0; j != 16; ++j) {
      raw_bytes[i] = raw_bytes[i] ^ rope[i * 16 + j];
    }
  }
}

std::string Knothash::to_string() const {
  std::string s;
  for (const std::uint8_t value : raw_bytes) {
    s += to_hex_char(value / 16);
    s += to_hex_char(value % 16);
  }
  return s;
}

Output day_2017_10(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const std::string line = lines[0];
  return Output(dei.can_skip_part_A ? 0 : part_a(line), Knothash(line).to_string());
}

// == DAY 14

void visit_group(const std::vector<std::bitset<128>> & map, bj::Position position, std::set<bj::Position> & grouped) {
  std::stack<bj::Position> to_visit;

  to_visit.emplace(position);
  grouped.emplace(position);

  while (!to_visit.empty()) {
    bj::Position pos = to_visit.top();
    to_visit.pop();

    pos.for_each_neighbour([&](bj::Position neighbour) {
      if (neighbour.x < 0 || neighbour.x >= 128) return;
      if (neighbour.y < 0 || std::cmp_greater_equal(neighbour.y, map.size())) return;
      if (!map[neighbour.y][neighbour.x]) return;
      if (grouped.contains(neighbour)) return;

      grouped.emplace(neighbour);
      to_visit.emplace(neighbour);
    });
  }
}

size_t count_number_of_areas(const std::vector<std::bitset<128>> & map) {
  std::set<bj::Position> grouped;
  size_t number_of_groups = 0;

  for (size_t line = 0; line != map.size(); ++line) {
    for (size_t i = 0; i != 128; ++i) {
      if (!map[line][i]) continue;

      bj::Position pos { static_cast<int>(i), static_cast<int>(line) };

      if (grouped.contains(pos)) continue;

      ++number_of_groups;

      visit_group(map, pos, grouped);
    }
  }

  return number_of_groups;
}

Output day_2017_14(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::string key = lines[0];

  std::vector<std::bitset<128>> the_map;

  size_t sum = 0;

  for (size_t i = 0; i != 128; ++i) {
    const std::string to_hash = key + "-" + std::to_string(i);
    const Knothash hash(to_hash);

    the_map.emplace_back(0);

    for (size_t i = 0; i != 16; ++i) {
      std::uint8_t byte = hash.raw_bytes[i];
      std::bitset<8> bitset(byte);

      for (size_t j = 0; j != 8; ++j) {
        if (bitset[j]) the_map.back().set((7 - j) + 8 * i, true);
      }

    }
    
    sum += the_map.back().count();
  }

  // the_map is flipped horizontally but we don't care because
  // we want to count the contiguous areas

  const size_t number_of_areas = count_number_of_areas(the_map);

  return Output(sum, number_of_areas);
}
