#include "../../common/advent_of_code.hpp"
#include <vector>
#include <exception>

// https://adventofcode.com/2018/day/09

template<typename T>
struct focused_ring {
  struct node {
    T element;
    node * previous;
    node * next;
  };

  node * focus;

  explicit focused_ring(T initial_element) {
    focus = new node();
    focus->element = std::move(initial_element);
    focus->previous = focus;
    focus->next = focus;
  }

  focused_ring(const focused_ring &) = delete;
  focused_ring & operator=(const focused_ring &) = delete;

  ~focused_ring() {
    while (focus->next != focus) {
      remove_focus();
    }
    delete focus;
  }

  T remove_focus() {
    if (focus->previous == focus) {
      throw std::runtime_error("Can't have an empty focused_ring");
    }

    node * to_delete = focus;
    focus = focus->previous;
    to_delete->next->previous = to_delete->previous;
    to_delete->previous->next = to_delete->next;
    focus->next = to_delete->next;
    T element = std::move(to_delete->element);
    delete to_delete;
    return element;
  }

  focused_ring & operator++()    { focus = focus->next;     return *this; }
  focused_ring & operator--()    { focus = focus->previous; return *this; }
  focused_ring & operator++(int) { focus = focus->next;     return *this; }
  focused_ring & operator--(int) { focus = focus->previous; return *this; }

  void insert_after_focus(T element) {
    node * new_node = new node();
    new_node->element = std::move(element);
    focus->next->previous = new_node;
    new_node->next = focus->next;
    focus->next = new_node;
    new_node->previous = focus;
  }

  T & operator*() { return focus->element; }
  const T & operator*() const { return focus->element; }
};


static std::int64_t play_the_marble_game(const size_t nb_players, const int last_marble) {
  std::vector<std::int64_t> points_per_players(nb_players, 0);

  focused_ring<int> circle(0);

  size_t current_player = 0;
  for (int marble = 1; marble <= last_marble; ++marble) {

    if (marble % 23 != 0) {
      ++circle;
      circle.insert_after_focus(marble);
      ++circle;
    } else {
      for(int i = 0; i != 7; ++i) {
        --circle;
      }

      const int earned_points = marble + circle.remove_focus();
      ++circle;
      points_per_players[current_player] += earned_points;
    }

    current_player = (current_player + 1) % nb_players;
  }


  return *std::max_element(points_per_players.begin(), points_per_players.end());
}


Output day_2018_09(const std::vector<std::string> & lines, const DayExtraInfo & dei) {
  const std::vector<int> values = bj::extract_ints_from_line(lines[0]);

  const size_t nb_players = static_cast<size_t>(values[0]);
  const int last_marble = values[1];

  const auto part_a = play_the_marble_game(nb_players, last_marble);

  if (dei.can_skip_part_B) return Output(part_a, 0);

  return Output(
    part_a,
    play_the_marble_game(nb_players, last_marble * 100)
  );
}
