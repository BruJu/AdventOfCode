#include "../advent_of_code.hpp"
#include <algorithm>
#include <vector>

// https://adventofcode.com/2022/day/20

// It is so awesome to have examples that are not representative of the input,
// looking at reddit to compare solutions, getting spoiled of the content of part B
// before finally seeing someone comments that the numbers are not 
// unique. Especially after day 19 that did not clearly specified that you can
// only buy one machine per minute.

namespace {

struct Node {
  long long int value;
  size_t original_position;
};

long long int compute_output(const std::vector<Node> & current) {
  const auto zero = std::find_if(current.begin(), current.end(),
    [](const Node & node) { return node.value == 0; }
  );

  const auto zero_pos = zero - current.begin();

  return current[(zero_pos + 1000) % current.size()].value
  + current[(zero_pos + 2000) % current.size()].value
  + current[(zero_pos + 3000) % current.size()].value;
}

std::vector<Node> read_input(const std::vector<std::string> & lines) {
  std::vector<Node> nodes;
  for (const std::string & line : lines) {
    nodes.emplace_back(Node{ std::stoi(line), nodes.size() });
  }
  return nodes;
}


// https://shadyf.com/blog/notes/2016-07-16-modulo-for-negative-numbers/
long long actually_functionning_mod(const long long k, const long long n) {
  return ( ( k % n ) + n ) % n;
}

void mix(std::vector<Node> & nodes) {
  for (size_t i = 0; i != nodes.size(); ++i) {
    const auto source = std::find_if(nodes.begin(), nodes.end(),
      [i](const Node & node) { return node.original_position == i; }
    );

    const Node node = *source;
    long long int destination = (source - nodes.begin() + node.value);

    nodes.erase(source);
    destination = actually_functionning_mod(destination, nodes.size());
    nodes.insert(nodes.begin() + destination, node);
  }
}

}

Output day_2022_20(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto part_a = [&]() {
    std::vector<Node> nodes = read_input(lines);
    mix(nodes);
    return compute_output(nodes);
  }();

  const auto part_b = [&]() {
    std::vector<Node> nodes = read_input(lines);
    for (Node & node : nodes) node.value *= 811589153;
    for (int i = 0; i != 10; ++i) mix(nodes);
    return compute_output(nodes);
  }();

  return Output(part_a, part_b);
}
