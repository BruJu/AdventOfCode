#include "../advent_of_code.hpp"
#include <map>

// https://adventofcode.com/2017/day/07

struct Info {
  std::string name;
  int weight;
  std::vector<std::string> children;

  Info * parent_ptr = nullptr;
  std::vector<Info *> children_ptr;

  int get_total_weight() const;
  [[nodiscard]] Info * is_unbalanced() const;
};

int Info::get_total_weight() const {
  int sum = 0;
  for (const Info * child : children_ptr) {
    sum += child->get_total_weight();
  }
  sum += weight;
  return sum;
}

Info * Info::is_unbalanced() const {
  if (children_ptr.size() <= 1) return nullptr;

  Info * first = children_ptr[0];
  Info * second = children_ptr[1];

  if (first->get_total_weight() != second->get_total_weight()) {
    if (children_ptr.size() == 2) return first;

    Info * third = children_ptr[2];
    if (first->get_total_weight() != third->get_total_weight()) {
      return first;
    } else {
      return second;
    }
  }

  const int ref = first->get_total_weight();
  for (Info * info : children_ptr) {
    if (info->get_total_weight() != ref) {
      return info;
    }
  }

  return nullptr;
}

Output day_2017_07(const std::vector<std::string> & lines, const DayExtraInfo &) {
  std::vector<Info> infos;

  for (const std::string & line : lines) {
    const auto words = bj::string_split(line, " ");

    Info info;
    info.name = words[0];
    info.weight = std::stoi(words[1].substr(1, words[1].size() - 2));
    for (size_t i = 3; i < words.size(); ++i) {
      std::string word = words[i];
      if (word.back() == ',') word = word.substr(0, word.size() - 1);
      info.children.emplace_back(word);
    }

    infos.emplace_back(std::move(info));
  }

  std::map<std::string, Info *> name_to_info;
  for (Info & info : infos) {
    name_to_info.emplace(info.name, &info);
  }

  for (Info & info : infos) {
    for (const std::string & child_name : info.children) {
      Info * child = name_to_info[child_name];
      child->parent_ptr = &info;
      info.children_ptr.emplace_back(child);
    }
  }

  Info * root = &infos[0];
  while (root->parent_ptr != nullptr) {
    root = root->parent_ptr;
  }

  // === PART B
  Info * unbalanced = root;
  int diff = 0;

  while (true) {
    Info * rip = unbalanced->is_unbalanced();
    if (!rip) break;

    Info * bad_child = rip->is_unbalanced();
    if (bad_child) {
      unbalanced = rip;
    } else {
      Info * other = unbalanced->children_ptr[0];
      if (other == rip) {
        other = unbalanced->children_ptr[1];
      }

      diff = - rip->get_total_weight() + other->get_total_weight();
      diff += rip->weight;
      break;
    }
  }

  return Output(root->name, diff);
}
