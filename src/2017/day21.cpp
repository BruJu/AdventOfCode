#include "../advent_of_code.hpp"
#include "../util/position.hpp"
#include <vector>
#include <set>
#include <map>

// https://adventofcode.com/2017/day/21

std::vector<bj::Position> to_positions(const std::string & token) {
  int x = 0;
  int y = 0;

  std::vector<bj::Position> retval;

  size_t i = 0;
  while (i < token.size()) {
    if (token[i] == '#') {
      retval.emplace_back(bj::Position{ x, y });
      ++x;
    } else if (token[i] == '.') {
      ++x;
    } else {
      ++y;
      x = 0;
    }

    ++i;
  }

  return retval;
}

int to_key(const std::vector<std::string> & form) {
  int key = 0;

  for (const std::string & form_line : form) {
    for (const char c : form_line) {
      key = key * 2;
      if (c == '#') {
        key += 1;
      }
    }
  }

  return key;
}

void rotate(std::vector<std::string> & form) {
  std::vector<std::string> res;
  for (size_t y = 0; y != form.size(); ++y) {
    res.emplace_back();
    for (size_t x = 0; x != form.size(); ++x) {
      res.back() += form[x][form.size() - y - 1];
    }
  }

  form = std::move(res);
}

void horizontal_flip(std::vector<std::string> & form) {
  for (size_t i = 0; i != form.size() / 2; ++i) {
    std::swap(form[i], form[form.size() - 1 - i]);
  }
}


void vertical_flip(std::vector<std::string> & form) {
  for (size_t i = 0; i != form.size() / 2; ++i) {
    for (size_t j = 0; j != form.size(); ++j) {
      std::swap(form[j][i], form[j][form.size() - 1 - i]);
    }
  }
}

template<typename BiConsumer>
void for_each_key(const std::string & token, BiConsumer biconsumer) {
  std::vector<std::string> like_earth = bj::string_split(token, "/");

  for (int h_flip = 0; h_flip != 2; ++h_flip) {
    for (int v_flip = 0; v_flip != 2; ++v_flip) {
      for (int rotation = 0; rotation != 4; ++rotation) {
        biconsumer(to_key(like_earth), like_earth.size());

        rotate(like_earth);
      }

      vertical_flip(like_earth);
    }

    horizontal_flip(like_earth);
  }

}

struct AllWrittenInTheManual {
  std::map<int, std::vector<bj::Position>> for2;
  std::map<int, std::vector<bj::Position>> for3;
};

struct Grid {
  std::set<bj::Position> lit;

  void next(const AllWrittenInTheManual & manual);

  [[nodiscard]] size_t size() const;

  void expand(const std::map<int, std::vector<bj::Position>> & pages, size_t size_, size_t placed_size);
};

size_t Grid::size() const {
  int x_max = 0;
  int y_max = 0;

  for (const bj::Position & position : lit) {
    x_max = std::max(position.x, x_max);
    y_max = std::max(position.y, y_max);
  }

  return std::max(x_max + 1, y_max + 1);
}


void Grid::next(const AllWrittenInTheManual & manual) {
  const auto size_ = size();

  if (size_ % 2 == 0) {
    expand(manual.for2, size_, 2);
  } else if (size_ % 3 == 0) {
    expand(manual.for3, size_, 3);
  } else {
    throw std::runtime_error("bad grid size");
  }
}

void Grid::expand(const std::map<int, std::vector<bj::Position>> & patterns, size_t size_, size_t pattern_size) {
  size_t number_of_placed_elements = size_ / pattern_size;

  std::set<bj::Position> new_grid;
  for (int big_x = 0; big_x != number_of_placed_elements; ++big_x) {
    for (int big_y = 0; big_y != number_of_placed_elements; ++big_y) {
      int key = 0;
      for (int y = 0; y != pattern_size; ++y) {
        for (int x = 0; x != pattern_size; ++x) {
          key *= 2;
          int X = big_x * pattern_size + x;
          int Y = big_y * pattern_size + y;
          if (lit.contains(bj::Position{ X, Y })) {
            key += 1;
          }
        }
      }

      const auto it = patterns.find(key);
      if (it == patterns.end()) {
        std::cerr << "key=" << key << "size=" << size_ << "\n";
        throw std::runtime_error("unfound key");
      }

      const auto & values = it->second;

      for (const bj::Position value : values) {
        int X = value.x + big_x * (pattern_size + 1);
        int Y = value.y + big_y * (pattern_size + 1);

        new_grid.emplace(bj::Position{ X, Y });
      }

    }
  }

  std::swap(new_grid, lit);
}

Output day_2017_21(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  AllWrittenInTheManual manual;

  for (const std::string & line : lines) {
    const auto tokens = bj::string_split(line, " => ");

    std::vector<bj::Position> enable = to_positions(tokens[1]);

    for_each_key(tokens[0], [&](int key, int size) {
      if (size == 2) {
        manual.for2[key] = enable;
      } else {
        manual.for3[key] = enable;
      }
    });
  }

  Grid grid;
  grid.lit.emplace(bj::Position{ 1, 0 });
  grid.lit.emplace(bj::Position{ 0, 1 });
  grid.lit.emplace(bj::Position{ 0, 2 });
  grid.lit.emplace(bj::Position{ 1, 2 });
  grid.lit.emplace(bj::Position{ 2, 2 });


  int until_a = dei.part_a_extra_param ? dei.part_a_extra_param : 5;

  for (int i = 0; i != until_a; ++i) {
    grid.next(manual);
  }

  const auto part_a = grid.lit.size();
  auto part_b = grid.lit.size();

  if (!dei.can_skip_part_B) {
    for (int i = until_a ; i != 18; ++i) {
      grid.next(manual);
    }

    part_b = grid.lit.size();
  }

  return Output(part_a, part_b);
}
