#include "../../common/advent_of_code.hpp"
#include "./2025.hpp"
#include <vector>
#include <span>
#include <utility>
#include <regex>
#include <ranges>
#include <algorithm>
#include <map>
#include "../../util/position.hpp"

// https://adventofcode.com/2025/day/6

// Reading numbers in column

namespace {

std::vector<char> parse_symbols(const std::string & line) {
  std::vector<char> symbols;
  for (const char c : line) {
    if (c != ' ') {
      symbols.push_back(c);
    }
  }
  return symbols;
}

std::int64_t do_part_a(const std::vector<std::string> & lines) {
  // Parsing
  if (lines.size() < 2) {
    throw std::runtime_error("202506 Not enough input lines");
  }

  std::vector<std::vector<std::int64_t>> number_lines;
  for (size_t i = 0; i + 1 < lines.size(); ++i) {
    number_lines.emplace_back(
      bj::extract_signed_i64_from_line(lines[i])
    );
  }

  for (const auto & nums : number_lines) {
    if (number_lines[0].size() != nums.size()) {
      std::cerr << std::format("Mismatched line sizes: expected {}, got {}\n", number_lines[0].size(), nums.size());
      throw std::runtime_error("202506a: Input lines have different lengths");
    }
  }

  const std::vector<char> line4 = parse_symbols(lines[lines.size() - 1]);

  // Computing
  std::int64_t part_a = 0;
  for (size_t i = 0; i != number_lines[0].size(); ++i) {
    const char        op = line4[i];

    std::int64_t pb_number;
    if (op == '*') {
      pb_number = 1;
      for (const auto & nums : number_lines) {
        pb_number *= nums[i];
      }
    } else if (op == '+') {
      pb_number = 0;
      for (const auto & nums : number_lines) {
        pb_number += nums[i];
      }
    } else {
      throw std::runtime_error("202506a: bad op");
    }

    part_a += pb_number;
  }

  return part_a;
}

std::int64_t do_part_b(std::vector<std::string> lines) {
  // This is full on boring input processing

  // 1/ Normalize length
  constexpr auto normalize_length = [](std::vector<std::string> & lines) {
    size_t max_length = 0;
    for (const std::string & line : lines) {
      max_length = std::max(max_length, line.size());
    }

    for (std::string & line : lines) {
      while (line.size() < max_length) {
        line.push_back(' ');
      }
    }
  };

  normalize_length(lines);

  // 2/ Code an accumulator for the see values during a column by column read
  struct Processor {
    std::vector<std::int64_t> numbers;
    char operator_ = '\0';

    [[nodiscard]] std::int64_t output() const {
      // To be able to call output mindlessly
      if (numbers.size() == 0) { return 0; }
      if (numbers.size() == 1) return numbers[0];

      // Several numbers, reduce them using the found operator
      if (operator_ == '*') {
        std::int64_t result = 1;
        for (const std::int64_t n : numbers) {
          result *= n;
        }
        return result;
      } else if (operator_ == '+') {
        std::int64_t result = 0;
        for (const std::int64_t n : numbers) {
          result += n;
        }
        return result;
      } else {
        throw std::runtime_error("202506b: bad op in processor " + std::to_string((int) operator_));
      }
    }

    void clear() {
      numbers.clear();
      operator_ = '\0';
    }
  };

  Processor processor;

  // 3/ Process each column

  std::int64_t sum = 0;

  // For each column ...
  for (size_t col = 0; col != lines[0].size(); ++col) {
    std::int64_t constructed_number = 0;
    bool seen_something = false;

    // See digit in each line and construct the number
    for (size_t row = 0; row != lines.size(); ++row) {
      char c = lines[row][col];

      if (c >= '0' && c <= '9') {
        constructed_number = constructed_number * 10 + (c - '0');
        seen_something = true;
      } else if (c == '+' || c == '*') {
        processor.operator_ = c;
        seen_something = true; // ? dubious line because a column is either fully empty or has at least one digit
      } else if (c == ' ') {
        // ok
      } else {
        throw std::runtime_error("202506b: unexpected character in input");
      }
    }

    if (seen_something) {
      // We saw a number (or a symbol but symbol only column does not exist), push the number
      processor.numbers.push_back(constructed_number);
    } else {
      // Empty column -> process what we saw so far
      sum += processor.output();
      processor.clear();
    }
  }

  sum += processor.output();
  return sum;
}


}

Output day_2025_06(const std::vector<std::string> & lines, const DayExtraInfo &) {

  return Output(do_part_a(lines), do_part_b(lines));
}
