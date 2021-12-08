#include "../advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <numeric>
#include <map>
#include <set>
#include <vector>

// https://adventofcode.com/2021/day/8

enum class Status {
  Yes, No, Unknown
};

static bool lhs_is_in_rhs(const std::string & lhs, const std::string & rhs) {
  if (lhs.size() >= rhs.size()) {
    std::cout << "wtf?\n";
  }

  return std::find_if(
    lhs.begin(),
    lhs.end(),
    [&](char char_in_lhs) {
      return std::find(rhs.begin(), rhs.end(), char_in_lhs) == rhs.end();
    }
  ) == lhs.end();
}

class LetterIdentifier {
  std::map<std::string, std::optional<int>> m_word_to_value;
  std::map<int, std::string> mapping;

public:
  void register_word(std::string word) {
    std::sort(word.begin(), word.end());

    if (word.size() == 2) m_word_to_value[word] = 1;
    else if (word.size() == 3) m_word_to_value[word] = 7;
    else if (word.size() == 4) m_word_to_value[word] = 4;
    else if (word.size() == 7) m_word_to_value[word] = 8;
    else m_word_to_value[word] = std::nullopt;
  }

  bool check_if_all_9() const {
    const auto s = m_word_to_value.size();
    return s == 10;
  }

  void determine() {
    if (!check_if_all_9()) std::cerr << "Error" << '\n';
    
    for (const auto & [word, number] : m_word_to_value) {
      if (number) {
        mapping[*number] = word;
      }
    }
    
    find_the_mapping_of_length_not_included_in(6, size_t(6), mapping[1]);
    find_the_mapping_of_length_not_included_in(0, size_t(6), mapping[4]);
    find_unmapped(9, size_t(6));

    find_the_mapping_of_length_that_includes(3, size_t(5), mapping[1]);
    find_the_mapping_of_length_not_included_in(2, size_t(5), mapping[9], true);
    find_unmapped(5, size_t(5));
  }

  int get(std::string word) {
    std::sort(word.begin(), word.end());

    return m_word_to_value[word].value();
  }

  void find_unmapped(int looked_for, size_t length) {
    bool found = false;

    for (auto & [cand_word, value] : m_word_to_value) {
      if (cand_word.size() != length) continue;
      if (value) continue;

      if (found) std::cerr << "Several unmapped of length " << length << '\n';
      found = true;

      value = looked_for;
      mapping[looked_for] = cand_word;
    }

    if (!found) {
      std::cerr << "Failed to find " << looked_for << '\n';
    }
  }

  void find_the_mapping_of_length_not_included_in(int looked_for, size_t length, std::string not_all, bool flip = false) {
    for (auto & [cand_word, value] : m_word_to_value) {
      if (cand_word.size() != length) continue;
      if (value) continue;

      const auto x = !flip ? !lhs_is_in_rhs(not_all, cand_word) : !lhs_is_in_rhs(cand_word, not_all);

      if (x) {
        value = looked_for;
        mapping[looked_for] = cand_word;
        return;
      }
    }

    std::cerr << "Failed to find " << looked_for << '\n';
  }

  void find_the_mapping_of_length_that_includes(int looked_for, size_t length, std::string to_find_in) {
    for (auto & [cand_word, value] : m_word_to_value) {
      if (cand_word.size() != length) continue;
      if (value) continue;

      const auto & cand_word_ = cand_word;

      if (lhs_is_in_rhs(to_find_in, cand_word_)) {
        value = looked_for;
        mapping[looked_for] = cand_word;
        return;
      }
    }

    std::cerr << "Failed to find " << looked_for << '\n';
  }

};

Output day_2021_08(const std::vector<std::string> & lines, const DayExtraInfo &) {

  long long int part_a = 0;
  long long int part_b = 0;

  for (const auto & line : lines) {
    const auto values = StringSplitter::line_to_vector<std::string>(
      line,
      [](std::string s) { return s; },
      ' '
    );

    size_t i = 0;
    while (values[i] != "|") ++i;
    ++i;

    // Part A
    for (size_t i = 11 ; i != values.size(); ++i) {
      const auto sz = values[i].size();

      if (sz == 2) ++part_a; // 1
      if (sz == 4) ++part_a; // 4
      if (sz == 3) ++part_a; // 7
      if (sz == 7) ++part_a; // 8
    }

    // Part B

    LetterIdentifier l;

    for (size_t i = 0; i != values.size(); ++i) {
      if (i == 10) continue;
      l.register_word(values[i]);
    }

    l.determine();

    part_b += l.get(values[11]) * 1000;
    part_b += l.get(values[12]) * 100;
    part_b += l.get(values[13]) * 10;
    part_b += l.get(values[14]) * 1;
  }

  return Output(part_a, part_b);
}
