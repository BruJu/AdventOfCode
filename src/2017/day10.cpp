#include "../advent_of_code.hpp"
#include <string>

// https://adventofcode.com/2017/day/10

struct RoundData {
  size_t current_position = 0;
  size_t skip_size = 0;

  void round(std::vector<int> & rope, size_t length) {
    if (length > rope.size()) {
      return;
    }

    reverse(rope, current_position, length);

    current_position = (current_position + length + skip_size) % rope.size();
    ++skip_size;
  }

  static void reverse(std::vector<int> & rope, size_t current_position, size_t length) {
    for (size_t i = 0; i != length / 2; ++i) {
      size_t left = (current_position + i) % rope.size();
      size_t right = (current_position + length - 1 - i) % rope.size();

      std::swap(rope[left], rope[right]);
    }
  }
};

int part_a(std::vector<int> rope, std::string line) {
  const std::vector<std::string> tokens = bj::string_split(line, ",");
  std::vector<size_t> lengths;
  for (const std::string & token : tokens) {
    lengths.emplace_back(std::stoul(token));
  }

  RoundData round_data;

  for (const size_t length : lengths) {
    round_data.round(rope, length);
  }

  return rope[0] * rope[1];
}

char to_hex_char(int number) {
  if (number >= 0 && number <= 9) return '0' + number;
  else return 'a' + number - 10;
}

std::string to_hex(int number) {
  return std::string("") + to_hex_char(number / 16) + to_hex_char(number % 16);
}

std::string part_b(std::vector<int> rope, std::string line) {
  std::vector<size_t> lengths;
  for (char c : line) {
    lengths.emplace_back(static_cast<size_t>(c));
  }

  for (const int extra : { 17, 31, 73, 47, 23 }) {
    lengths.emplace_back(extra);
  }

  RoundData round_data;

  for (int i = 0; i != 64; ++i) {
    for (const size_t length : lengths) {
      round_data.round(rope, length);
    }
  }

  std::string str;
  for (int i = 0; i != 16; ++i) {
    int xor_ = rope[i * 16];
    for (int j = 1; j != 16; ++j) {
      xor_ = xor_ ^ rope[i * 16 + j];
    }
    str += to_hex(xor_);
  }

  return str;
}

Output day_2017_10(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const std::string line = lines[0];

  std::vector<int> rope;
  for (int i = 0; i != 256; ++i) {
    rope.emplace_back(i);
  }

  return Output(
    dei.can_skip_part_A ? 0 : part_a(rope, line),
    part_b(rope, line)
  );
}
