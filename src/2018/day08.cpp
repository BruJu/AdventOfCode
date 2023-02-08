#include "../advent_of_code.hpp"
#include <memory>
#include <vector>

// https://adventofcode.com/2018/day/08


namespace {

struct Node {
  std::vector<std::unique_ptr<Node>> m_children;
  std::vector<int> m_metadata;

  [[nodiscard]] std::int64_t get_sum_of_meta() const {
    std::int64_t metadata_sum = 0;

    for (const auto & child : m_children) {
      metadata_sum += child->get_sum_of_meta();
    }

    for (const int meta : m_metadata) {
      metadata_sum += meta;
    }

    return metadata_sum;
  }

  [[nodiscard]] std::int64_t get_value() const {
    if (m_children.empty()) {
      return get_sum_of_meta();
    }

    std::int64_t value = 0;

    for (const int meta : m_metadata) {
      if (meta <= 0) continue;
      size_t child_index = static_cast<size_t>(meta - 1);

      if (child_index < m_children.size()) {
        value += m_children[child_index]->get_value();
      }
    }

    return value;
  }
};

struct Reader {
  const std::vector<int> * values;
  size_t cursor = 0;

  [[nodiscard]] Node get_node() {
    if (cursor == values->size()) return Node{};
    
    const std::vector<int> & values = *this->values;

    int nb_children = values[cursor];
    int nb_metadata = values[cursor + 1];

    cursor += 2;

    Node retval;
    for (int i = 0; i != nb_children; ++i) {
      retval.m_children.emplace_back(std::make_unique<Node>(get_node()));
    }

    for (int i = 0; i != nb_metadata; ++i) {
      retval.m_metadata.emplace_back(values[cursor]);
      ++cursor;
    }

    return retval;
  }
};

}


Output day_2018_08(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const std::vector<int> values = bj::extract_ints_from_line(lines[0]);

  Reader reader { &values, 0 };

  Node root = reader.get_node();

  const std::int64_t sum_of_all_meta = root.get_sum_of_meta();
  const std::int64_t root_value = root.get_value();

  return Output(sum_of_all_meta, root_value);
}
