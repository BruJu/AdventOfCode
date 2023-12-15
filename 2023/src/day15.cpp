#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <string_view>
#include <charconv>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/15


namespace {

int to_int(auto begin, auto end) {
  int value;
  std::from_chars(&*begin, &*end, value);
  return value;
}

i64 compute_HASH(const std::string_view s) {
  i64 val = 0;

  for (const char c : s) {
    val = ((val + c) * 17) % 256;
  }

  return val;
}

struct Boxes {
  struct Lens {
    std::string name;
    int value;
  };

  struct Box {
    std::vector<Lens> lenses;
    
  private:
    auto find(const std::string_view name) {
      return std::find_if(lenses.begin(), lenses.end(), [&](const Lens & lens) {
        return lens.name == name;
      });
    }

  public:
    void add_lens(const std::string_view name, int value) {
      const auto it = find(name);
      if (it == lenses.end()) {
        lenses.emplace_back(Lens{ std::string(name), value });
      } else {
        it->value = value;
      }
    }

    void remove_lens(const std::string_view name) {
      const auto it = find(name);
      if (it != lenses.end()) {
        lenses.erase(it);
      }
    }

    [[nodiscard]] int get_focusing_power() const noexcept {
      int sum = 0;
      for (size_t i = 0; i != lenses.size(); ++i) {
        sum += (static_cast<int>(i) + 1) * lenses[i].value;
      }
      return sum;
    }
  };

public:
  std::array<Box, 256> boxes;

  void read_instruction(const std::string & instruction) {
    if (const size_t equal = instruction.find("="); equal != std::string::npos) {
      const std::string_view name{ instruction.begin(), instruction.begin() + equal };
      const int value = to_int(instruction.begin() + equal + 1, instruction.end());
      
      const int box_id = compute_HASH(name);
      boxes[box_id].add_lens(name, value);
    } else {
      const std::string_view name = std::string_view(instruction.begin(), instruction.begin() + instruction.size() - 1);
      
      const int box_id = compute_HASH(name);
      boxes[box_id].remove_lens(name);
    }
  }

  [[nodiscard]] i64 get_focusing_power() const noexcept {
    i64 sum = 0;
    for (int i = 0; i != 256; ++i) {
      sum += (i + 1) * boxes[i].get_focusing_power();
    }
    return sum;
  }
};

}

Output day_2023_15(const std::vector<std::string> & lines, const DayExtraInfo &) { 
  i64 part_a = 0;

  Boxes boxes;

  for (const std::string & instruction : bj::string_split(lines[0], ",")) {
    part_a += compute_HASH(instruction);

    boxes.read_instruction(instruction);
  }

  const i64 part_b = boxes.get_focusing_power();

  return Output(part_a, part_b);
}
