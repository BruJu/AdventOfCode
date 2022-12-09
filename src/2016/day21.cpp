#include "../advent_of_code.hpp"
#include <map>
#include <exception>
#include <ranges>
#include "../util/instruction_reader.hpp"
#include <variant>
#include <regex>
#include <span>

// https://adventofcode.com/2016/day/21
// Do complicated things on words
// (A) run the instructions
// (B) unrun the instructions

struct SwapPosition {
  static constexpr const char * Regex = R"(swap position ([0-9]) with position ([0-9]))";

  size_t pos_a;
  size_t pos_b;

  explicit SwapPosition(const std::vector<std::string> & values)
    : pos_a(std::stoul(values[0]))
    , pos_b(std::stoul(values[1])) {
    }

  void apply(std::string & word) const { std::swap(word[pos_a], word[pos_b]); }
  void unapply(std::string & word) const { apply(word); }
};

struct SwapLetter {
  static constexpr const char * Regex = R"(swap letter ([a-z]) with letter ([a-z]))";

  char letter_a;
  char letter_b;

  explicit SwapLetter(const std::vector<std::string> & values)
    : letter_a(values[0][0])
    , letter_b(values[1][0]) {
    }

  void apply(std::string & word) const {
    const auto pos_a = word.find(letter_a);
    const auto pos_b = word.find(letter_b);
    std::swap(word[pos_a], word[pos_b]);
  }

  void unapply(std::string & word) const { apply(word); }
};

struct RotateLeft {
  static constexpr const char * Regex = R"(rotate left ([0-9]) steps?)";

  size_t steps;

  explicit RotateLeft(const std::vector<std::string> & values)
    : steps(std::stoul(values[0])) {
    }

  explicit RotateLeft(size_t steps) : steps(steps) {}

  void apply(std::string & word) const {
    word = word.substr(steps) + word.substr(0, steps);
  }
  void unapply(std::string & word) const {
    const size_t real_step = word.size() - steps;
    word = word.substr(real_step) + word.substr(0, real_step);
  }
};

struct RotateRight {
  static constexpr const char * Regex = R"(rotate right ([0-9]) steps?)";

  RotateLeft base;

  explicit RotateRight(const std::vector<std::string> & values)
    : base(std::stoul(values[0])) {}

  void apply(std::string & word) const { base.unapply(word); }
  void unapply(std::string & word) const { base.apply(word); }
};

struct RotateLetter {
  static constexpr const char * Regex = R"(rotate based on position of letter ([a-z]))";

  char letter;

  explicit RotateLetter(const std::vector<std::string> & values)
    : letter(values[0][0]) {}

  void apply(std::string & word) const {
    int pos = word.find(letter);
    if (pos >= 4) pos += 2; else pos += 1;
    pos = pos % word.length();
    if (pos == 0) return;
    const int steps = word.size() - pos;
    word = word.substr(steps) + word.substr(0, steps);
  }

  void unapply(std::string & word) const {
    // Coming up with the actual unapply function is hard.
    // But we know all the possibles inputs x for apply
    // and we need to find an x such that apply(x) = word
    // here comes BRUTEFORCE
    std::string cpy = word;

    for (size_t i = 0; i != word.length(); ++i) {
      // Compute a rotation
      std::string cand = word.substr(i) + word.substr(0, i);

      // What is its result?
      std::string cand_app = cand;
      apply(cand_app);

      if (cand_app == word) {
        word = cand;
        return;
      }
    }

    // Tested all possible rotations but no result
  }
};

struct ReversePositions {
  static constexpr const char * Regex = R"(reverse positions ([0-9]) through ([0-9]))";

  size_t pos_a;
  size_t pos_b;

  explicit ReversePositions(const std::vector<std::string> & values)
    : pos_a(std::stoul(values[0]))
    , pos_b(std::stoul(values[1])) {
    }

  void apply(std::string & word) const {
    size_t a = pos_a;
    size_t b = pos_b;

    while (a < b && b != size_t(-1)) {
      std::swap(word[a], word[b]);
      ++a;
      --b;
    }
  }

  void unapply(std::string & word) const { apply(word); }
};

struct MovePosition {
  static constexpr const char * Regex = R"(move position ([0-9]) to position ([0-9]))";

  size_t from;
  size_t to;

  explicit MovePosition(const std::vector<std::string> & values)
    : from(std::stoul(values[0]))
    , to(std::stoul(values[1])) {
    }

  void apply(std::string & word) const {
    const char c = word[from];
    word.erase(word.begin() + from);
    word.insert(word.begin() + to, c);
  }

  void unapply(std::string & word) const {
    const char c = word[to];
    word.erase(word.begin() + to);
    word.insert(word.begin() + from, c);
  }
};

using Instruction = std::variant<
  SwapPosition, SwapLetter, RotateLeft, RotateRight, RotateLetter, ReversePositions, MovePosition
>;

template<typename Type>
void add_handler_for(bj::InstructionReader<Instruction> & reader) {
  reader.add_handler(
    Type::Regex,
    [](const std::vector<std::string> & values) -> Instruction {
      return Type(values);
    }
  );
}

Output day_2016_21(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::string to_scramble = lines[0];
  std::string to_unscramble = lines[1];

  const std::span<const std::string> instruction_lines(lines.begin() + 2, lines.end());

  bj::InstructionReader<Instruction> reader;
  add_handler_for<SwapPosition>(reader);
  add_handler_for<SwapLetter>(reader);
  add_handler_for<RotateLeft>(reader);
  add_handler_for<RotateRight>(reader);
  add_handler_for<RotateLetter>(reader);
  add_handler_for<ReversePositions>(reader);
  add_handler_for<MovePosition>(reader);

  const auto opt_instructions = reader(instruction_lines);
  if (!opt_instructions.has_value()) return Output(0, 0);

  for (const Instruction & instruction : opt_instructions.value()) {
    std::visit([&](auto && arg) { arg.apply(to_scramble); }, instruction);
  }

  if (to_unscramble != "_") {
    size_t i = opt_instructions->size();
    for (const Instruction & instruction : opt_instructions.value() | std::views::reverse) {
      std::string before = to_unscramble;
      
      std::visit([&](auto && arg) { arg.unapply(to_unscramble); }, instruction);

      std::string to_check = to_unscramble;

      std::visit([&](auto && arg) { arg.apply(to_check); }, instruction);

      if (before != to_check) {
        std::cerr << "Error at line " << (i + 2) << "\n";
      }

      --i;
    }
  }

  return Output(to_scramble, to_unscramble);
}
