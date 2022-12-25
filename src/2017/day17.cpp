#include "../advent_of_code.hpp"
#include <vector>
#include <map>
#include <memory>

// https://adventofcode.com/2017/day/17

namespace {

int part_a(int offset) {
  std::vector<int> circular_buffer;
  circular_buffer.emplace_back(0);

  // Part A
  size_t where = 0;
  for (int i = 1; i != 2018; ++i) {
    where = (where + offset) % circular_buffer.size();
    circular_buffer.insert(circular_buffer.begin() + where + 1, i);
    ++where;
  }

  auto it = std::find(circular_buffer.begin(), circular_buffer.end(), 2017) - circular_buffer.begin();
  return circular_buffer[(it + 1) % circular_buffer.size()];
}

struct LinkedList {
  LinkedList * previous;
  LinkedList * next;
  int value;

  static LinkedList * new_element(std::vector<std::unique_ptr<LinkedList>> & elements, int value);

  LinkedList * advance(int offset);
  LinkedList * find(int wanted_value);
};

LinkedList * LinkedList::advance(int offset) {
  if (offset == 0) return this;
  return next->advance(offset - 1);
}

LinkedList * LinkedList::find(int wanted_value) {
  if (value == wanted_value) return this;
  return next->find(wanted_value);
}

LinkedList * LinkedList::new_element(std::vector<std::unique_ptr<LinkedList>> & elements, int value) {
  LinkedList * elem = new LinkedList();
  elements.emplace_back(elem);
  elem->value = value;
  return elem;
}

int part_b(int offset) {
  std::vector<std::unique_ptr<LinkedList>> elements;
  
  LinkedList * current = LinkedList::new_element(elements, 0);
  current->previous = current;
  current->next = current;

  for (int i = 1; i != 50000001; ++i) {
    current = current->advance(offset);
    LinkedList * previous = current;
    LinkedList * next = current->next;

    current = LinkedList::new_element(elements, i);
    previous->next = current;
    next->previous = current;
    current->previous = previous;
    current->next = next;
  }

  current = current->find(0);
  return current->next->value;
}

}

Output day_2017_17(const std::vector<std::string> &, const DayExtraInfo & dei) {
  int offset = dei.part_a_extra_param;
  
  const auto result_a = part_a(offset);
  if (dei.can_skip_part_B) return Output(result_a, 0);

  const auto result_b = part_b(offset);
  return Output(result_a, result_b);
}
