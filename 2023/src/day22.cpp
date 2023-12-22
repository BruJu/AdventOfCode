#include "../../common/advent_of_code.hpp"
#include <vector>
#include <map>
#include "../../util/instruction_reader.hpp"
#include <set>
#include "../../util/position.hpp"
#include <compare>
#include <ranges>
#include <algorithm>

using i64 = std::int64_t;

// https://adventofcode.com/2023/day/22


struct Interval {
  int min;
  int max;

  Interval(int a, int b) {
    min = a < b ? a : b;
    max = a > b ? a : b;
  }

  [[nodiscard]] bool overlaps(const Interval & other) const {
    return min <= other.max && other.min <= max;
  }

  friend std::ostream & operator<<(std::ostream & s, const Interval & i) {
    return s << "[" << i.min << "~" << i.max << "]";
  }

  constexpr auto operator<=>(const Interval &) const noexcept = default;
};

struct Brick {
  Interval x;
  Interval y;
  Interval z;

  Brick(int x1, int y1, int z1, int x2, int y2, int z2)
  : x(x1, x2), y(y1, y2), z(z1, z2) {
  }

  bool supports(const Brick & other) const {
    if (z.max + 1 != other.z.min) return false;
    return x.overlaps(other.x) && y.overlaps(other.y);
  }

  friend std::ostream & operator<<(std::ostream & s, const Brick & b) {
    return s << "x=" << b.x << ",y=" << b.y << ",z=" << b.z;
  }

  void move_down() { --z.min; --z.max; }

  constexpr auto operator<=>(const Brick &) const noexcept = default;
};

void print(std::span<const Brick> bricks) {
  for (const Brick & b : bricks) {
    std::cout << b << "\n";
  }
  std::cout << "\n";
}

void cosmic_fall(std::span<Brick> bricks) {
  bool unstable;
  do {
    unstable = false;
    for (Brick & falling : bricks) {
      while (true) {
        // Support from ground
        if (falling.z.min <= 1) break;

        bool has_support = false;

        // Support from other brick
        for (Brick & support : bricks) {
          if (&falling == &support) continue;

          if (support.supports(falling)) {
            has_support = true;
            break;
          }
        }

        if (has_support) break;

        // Move down
        unstable = true;
        falling.move_down();
      }
    }
  } while (unstable);
}

std::map<const Brick *, std::set<const Brick *>>
get_supported_to_supporter(std::span<const Brick> bricks) {
  std::map<const Brick *, std::set<const Brick *>> supported_to_supporter;

  for (const Brick & block : bricks) {
    for (const Brick & support : bricks) {
      if (&block == &support) continue;

      if (support.supports(block)) {
        supported_to_supporter[&block].emplace(&support);
      }
    }
  }

  return supported_to_supporter;
}

size_t count_part_a(std::span<Brick> bricks) {
  const auto supported_to_supporter = get_supported_to_supporter(bricks);

  return std::ranges::count_if(
    bricks,
    [&](const Brick & brick) {
      return std::ranges::none_of(supported_to_supporter | std::views::values,
        [&](const auto & supporters) {
          return supporters.size() == 1 && supporters.contains(&brick);
        }
      );
    }
  );
}


i64 number_of_moved_blocks_if_erased(std::span<const Brick> bricks, size_t i) {
  std::vector<Brick> copy(bricks.begin(), bricks.end());

  copy[i] = Brick(-1,-1,-1,-1,-1,-1);

  cosmic_fall(copy);

  i64 changed = 0;
  for (size_t i = 0; i != bricks.size(); ++i) {
    if (bricks[i] != copy[i]) {
      ++changed;
    }
  }

  return changed - 1;
}

[[maybe_unused]] i64 do_part_b(std::span<const Brick> bricks) {
  // Naive approach for part B that consists in just removing each block
  // and do the cosmic fall thingy.
  // It would be more efficient to use the chart of dependencies
  i64 part_b = 0;
  for (size_t i = 0; i != bricks.size(); ++i) {
    part_b += number_of_moved_blocks_if_erased(bricks, i);
  }
  return part_b;
}


[[maybe_unused]] i64 do_part_b_but_better(std::span<const Brick> bricks) {
  const auto supported_to_supporter = get_supported_to_supporter(bricks);

  i64 result = 0;

  for (size_t i = 0; i != bricks.size(); ++i) {
    std::set<const Brick *> erased;
    erased.emplace(&bricks[i]);

    bool stable;
    do {
      stable = true;

      for (const auto & [supported, supporters] : supported_to_supporter) {
        if (erased.contains(supported)) continue;

        if (std::ranges::includes(erased, supporters)) {
          erased.emplace(supported);
          stable = false;
        }
      }      
    } while (!stable);

    result += erased.size() - 1;
  }

  return result;
}

Output day_2023_22(const std::vector<std::string> & lines, const DayExtraInfo & dei) {

  bj::InstructionReader<Brick> reader;

  reader.add_handler(
    R"(^(\d*),(\d*),(\d*)~(\d*),(\d*),(\d*)$)",
    [](const std::vector<std::string> & v) -> Brick {
      return Brick(
        std::stoi(v[0]),
        std::stoi(v[1]),
        std::stoi(v[2]),
        std::stoi(v[3]),
        std::stoi(v[4]),
        std::stoi(v[5])
      );
    }
  );

  std::vector<Brick> bricks = reader(lines).value();

  cosmic_fall(bricks);


  const i64 part_a = static_cast<i64>(count_part_a(bricks));
  i64 part_b = do_part_b(bricks);

  return Output(part_a, part_b);
}
