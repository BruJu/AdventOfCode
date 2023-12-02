#include "../../common/advent_of_code.hpp"
#include <set>
#include "../../util/position.hpp"

// https://adventofcode.com/2018/day/13

enum class Tile {
  Continue,
  ToNE,
  ToNW,
  Intersection,
  Empty
};

struct Rails {
  std::vector<std::string> road;

  [[nodiscard]] Tile get_type(bj::Position position) const {
    const char c = road[position.y][position.x];
    switch (c) {
      case '/': return Tile::ToNE;
      case '\\': return Tile::ToNW;
      case '|':
      case '-':
        return Tile::Continue;
      case '+':
        return Tile::Intersection;
      case ' ':
        return Tile::Empty;
      default:
        throw std::runtime_error("unknown tile type");
    }
  }
};

struct Cart : public bj::Kitten {
  int sequence = 0;

  void move(const Rails & rails);
};

void Cart::move(const Rails & rails) {
  advance();

  const Tile tile = rails.get_type(position);

  switch (tile) {
    case Tile::ToNE: {
      switch (direction) { // /
        case bj::Direction::Left : direction = bj::Direction::Down;  break;
        case bj::Direction::Down : direction = bj::Direction::Left;  break;
        case bj::Direction::Right: direction = bj::Direction::Top;   break;
        case bj::Direction::Top  : direction = bj::Direction::Right; break;
      }
      break;
    }
    case Tile::ToNW: {
      switch (direction) { // '\' 
        case bj::Direction::Left : direction = bj::Direction::Top;   break;
        case bj::Direction::Top  : direction = bj::Direction::Left;  break;
        case bj::Direction::Right: direction = bj::Direction::Down;  break;
        case bj::Direction::Down : direction = bj::Direction::Right; break;
      }
      break;
    }
    case Tile::Intersection: {
      if (sequence == 0) {
        turn_left();
      } else if (sequence == 2) {
        turn_right();
      }
      sequence = (sequence + 1) % 3;
      break;
    }
    case Tile::Empty: {
      throw std::runtime_error("Cart on empty slot");
      break;
    }
    default:
      break;
  }
}

static std::pair<Rails, std::vector<Cart>> read_input(std::vector<std::string> lines) {
  std::vector<Cart> carts;

  for (int y = 0; std::cmp_less(y, lines.size()); ++y) {
    for (int x = 0; std::cmp_less(x, lines[y].size()); ++x) {
      char & c = lines[y][x];
      std::optional<bj::Direction> dir;
      if (c == '>') { dir = bj::Direction::Right; c = '-'; }
      if (c == '<') { dir = bj::Direction::Left;  c = '-'; }
      if (c == '^') { dir = bj::Direction::Top;   c = '|'; }
      if (c == 'v') { dir = bj::Direction::Down;  c = '|'; }

      if (dir) {
        carts.emplace_back(Cart { bj::Kitten(bj::Position{ x, y }, dir.value()) });
      }
    }
  }

  return std::pair<Rails, std::vector<Cart>>(
    Rails { lines },
    carts
  );
}

static std::vector<bj::Position> tick(const Rails & rails, std::vector<Cart> & carts) {
  std::vector<bj::Position> crashes;

  std::set<bj::Position> positions_with_cart;

  for (const Cart & cart : carts) {
    positions_with_cart.emplace(cart.position);
  }

  std::sort(
    carts.begin(), carts.end(),
    [](const Cart & lhs, const Cart & rhs) {
      return std::pair(lhs.position.y, lhs.position.x)
        < std::pair(rhs.position.y, rhs.position.x);
    }
  );

  auto it_cart = carts.begin();
  while (it_cart != carts.end()) {
    positions_with_cart.erase(it_cart->position);
    it_cart->move(rails);

    if (positions_with_cart.contains(it_cart->position)) {
      const bj::Position pos = it_cart->position;
      positions_with_cart.erase(it_cart->position);
      it_cart = carts.erase(it_cart);
      auto other_cart = std::find_if(carts.begin(), carts.end(),
        [pos](const Cart & cart) { return pos == cart.position; }
      );
      if (other_cart < it_cart) {
        --it_cart; // It maybe is UB idk
      }
      carts.erase(other_cart);
      crashes.emplace_back(pos);
    } else {
      positions_with_cart.emplace(it_cart->position);
      ++it_cart;
    }
  }

  return crashes;
}

static std::string to_string(const bj::Position position) {
  return std::to_string(position.x) + ',' + std::to_string(position.y);
}

Output day_2018_13(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto p = read_input(lines);

  const Rails rails = p.first;
  std::vector<Cart> carts = p.second;

  std::optional<bj::Position> first_crash;
  while (carts.size() > 1) {
    const auto crashes = tick(rails, carts);
    if (!crashes.empty() && !first_crash) {
      first_crash = crashes[0];
    }
  }

  const std::string part_a = to_string(first_crash.value());
  const std::string part_b = carts.empty() ? "/" : to_string(carts[0].position);

  return Output(part_a, part_b);
}
