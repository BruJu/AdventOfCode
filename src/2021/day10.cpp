#include "../advent_of_code.hpp"
#include <algorithm>
#include <vector>
#include <stack>
#include <tuple>

// https://adventofcode.com/2021/day/10

enum class Type { Opening, Closing };

static std::tuple<Type, char, int> get_operation(char c) {
  if (c == '(') return std::tuple(Type::Opening, ')', 3);
  if (c == ')') return std::tuple(Type::Closing, ')', 3);

  if (c == '[') return std::tuple(Type::Opening, ']', 57);
  if (c == ']') return std::tuple(Type::Closing, ']', 57);

  if (c == '{') return std::tuple(Type::Opening, '}', 1197);
  if (c == '}') return std::tuple(Type::Closing, '}', 1197);
  
  if (c == '<') return std::tuple(Type::Opening, '>', 25137);
  if (c == '>') return std::tuple(Type::Closing, '>', 25137);

  return std::tuple(Type::Closing, c, 1000000);
}

static long long int get_completition_score(char c) {
  if (c == ')') return 1;
  if (c == ']') return 2;
  if (c == '}') return 3;
  if (c == '>') return 4;
  return -1;
}

static long long int syntax_error(const std::string & line, std::stack<char> & stack) {
  for (char c : line) {
    const auto [type, symbol, points] = get_operation(c);

    if (type == Type::Opening) {
      stack.emplace(symbol);
    } else {
      if (stack.empty()) return 1000000; // ??? Should not happen according to the text

      const auto on_top = stack.top();
      stack.pop();

      if (on_top != symbol) return points;
    }
  }

  return 0;
}

Output day_2021_10(const std::vector<std::string> & lines, const DayExtraInfo &) {
  long long int part_a = 0;

  std::vector<long long int> autocompletition_scores;

  std::stack<char> stack;
  for (const auto & line : lines) {
    const auto syntax_error_score = syntax_error(line, stack);

    if (syntax_error_score != 0) {
      part_a += syntax_error_score;
      while (!stack.empty()) stack.pop();
    } else {
      long long int completition_score = 0;

      while (!stack.empty()) {
        const char next_missing = stack.top();
        stack.pop();

        completition_score = completition_score * 5 + get_completition_score(next_missing);
      }

      autocompletition_scores.push_back(completition_score);
    }
  }

  std::sort(autocompletition_scores.begin(), autocompletition_scores.end());
  const long long int part_b = autocompletition_scores[autocompletition_scores.size() / 2];
  return Output(part_a, part_b);
}
