#include "../../common/advent_of_code.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

// https://adventofcode.com/2025/day/7

namespace {

void assert(bool something, std::string id) {
  if (!something) throw std::runtime_error("assert is wrong, and I should use a real assert function " + id);
}

struct Box {
  std::int64_t x;
  std::int64_t y;
  std::int64_t z;

  explicit Box(const std::string & str) {
    std::vector<std::int64_t> vals = bj::extract_signed_i64_from_line(str);
    if (vals.size() != 3) throw std::runtime_error("202508 bad input");
    x = vals[0];
    y = vals[1];
    z = vals[2];
  }

  [[nodiscard]] static
  std::vector<Box> from_strings(std::span<const std::string> lines) {
    std::vector<Box> junction_boxes;
    for (const std::string & line : lines) {
      junction_boxes.emplace_back(line);
    }
    return junction_boxes;
  }

  [[nodiscard]]
  std::int64_t distance_squared(const Box & other) const {
    // We assume that values are <<< 1 million so the square fits in an int64

    std::int64_t delta_x = x - other.x; delta_x *= delta_x;
    std::int64_t delta_y = y - other.y; delta_y *= delta_y;
    std::int64_t delta_z = z - other.z; delta_z *= delta_z;

    return delta_x + delta_y + delta_z;
  }

  [[nodiscard]] std::string str() const {
    return std::format("{},{},{}", x, y, z);
  }

};

struct Distance {
  std::int64_t length;
  size_t a;
  size_t b;

  [[nodiscard]] static
  std::vector<Distance> build_from_boxes(std::span<const Box> boxes) {
    std::vector<Distance> distances;

    for (size_t a = 0; a != boxes.size(); ++a) {
      const Box & box_a = boxes[a];
      for (size_t b = a + 1; b != boxes.size(); ++b) {
        const Box & box_b = boxes[b];
        distances.emplace_back(Distance { box_a.distance_squared(box_b), a, b });
      }
    }

    std::sort(
      distances.begin(),
      distances.end(),
      [](const Distance & lhs, const Distance & rhs) {
        return std::tie(lhs.length, lhs.a, lhs.b) <
              std::tie(rhs.length, rhs.a, rhs.b);
      }
    );

    return distances;
  }
};

struct UnionFind {
private:
  std::vector<size_t> parent;

public:
  explicit UnionFind(size_t length) {
    parent.reserve(length);
    for (size_t i = 0; i != length; ++i) {
      parent.emplace_back(i);
    }
  }

  [[nodiscard]] bool has_same_root(size_t a, size_t b) const {
    return (*this)[a] == (*this)[b];
  }
    
  void connect(size_t a, size_t b) {
    size_t root_a = (*this)[a];
    size_t root_b = (*this)[b];
    parent[root_a] = root_b;
  }

  [[nodiscard]] size_t operator[](size_t i) const {
    while (i != parent[i]) {
      i = parent[i];
    }

    return i;
  }
    

  std::vector<size_t> get_size_of_circuits() {
    compress_paths();

    std::vector<size_t> sizes;
    for (size_t i = 0; i != parent.size(); ++i) {
      if (parent[i] != i) continue;

      sizes.emplace_back(std::count(parent.begin(), parent.end(), i));
    }

    return sizes;
  }

private:
  void compress_paths() {
    for (size_t i = 0; i != parent.size(); ++i) {
      parent[i] = (*this)[i];
    }
  }
};

std::int64_t compute_part_a_output(UnionFind & union_find) {
  std::vector<size_t> sizes = union_find.get_size_of_circuits();
  std::sort(sizes.rbegin(), sizes.rend());

  assert(sizes.size() >= 3, "compute_part_a_output");
  return sizes[0] * sizes[1] * sizes[2];
}


}

Output day_2025_08(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const std::vector<Box> junction_boxes = Box::from_strings(lines);
  const std::vector<Distance> distances = Distance::build_from_boxes(junction_boxes);

  UnionFind union_find(junction_boxes.size());

  // -- PART A
  // The problem is so badly worded, that's unreal. Not making a connection counts as making a connection.
  size_t number_of_done_connections = 0;
  for (int connexion_number = 0; connexion_number != dei.part_a_extra_param; ++connexion_number) {
    const Distance & distance = distances[connexion_number];

    if (union_find.has_same_root(distance.a, distance.b)) {
      // do nothing
    } else {
      union_find.connect(distance.a, distance.b);
      ++number_of_done_connections;
    }
  }

  const std::int64_t part_a = compute_part_a_output(union_find);

  // -- PART B
  std::int64_t part_b = -1;
  {
    int connexion_number = dei.part_a_extra_param + 1;

    while (number_of_done_connections + 1 != junction_boxes.size()) {
      const Distance & distance = distances[connexion_number];

      if (union_find.has_same_root(distance.a, distance.b)) {
        // do nothing
      } else {
        union_find.connect(distance.a, distance.b);
        ++number_of_done_connections;
      }

      ++connexion_number;
    }

    --connexion_number;
    part_b = junction_boxes[distances[connexion_number].a].x * junction_boxes[distances[connexion_number].b].x;
  }

  // Return result
  return Output(part_a, part_b);
}
