#include "../../common/advent_of_code.hpp"
#include <algorithm>
#include <optional>
#include <map>
#include <vector>

// https://adventofcode.com/2021/day/8

// We use the fact that in each line, every number is sent.
// word = set of segments

/** Return true if all letters in lhs are contained in rhs*/
static bool lhs_is_in_rhs(const std::string & lhs, const std::string & rhs) {
  if (lhs.size() >= rhs.size()) {
    std::cout << "lhs_is_in_rhs(" << lhs << ", " << rhs << ") -> lhs is not shorter than rhs\n";
  }

  return std::find_if(
    lhs.begin(),
    lhs.end(),
    [&](char char_in_lhs) {
      return std::find(rhs.begin(), rhs.end(), char_in_lhs) == rhs.end();
    }
  ) == lhs.end();
}

/**
 * The class that builds back the mapping from sorted messages to number.
 * 
 * This class is badly designed, but it is only used here so whatever.
 */
class LetterIdentifier {
  std::map<std::string, std::optional<int>> m_word_to_value;
  std::map<int, std::string> mapping;

public:
  /** Register a sent word */
  void register_word(std::string word) {
    std::sort(word.begin(), word.end());

    if (word.size() == 2) m_word_to_value[word] = 1;
    else if (word.size() == 3) m_word_to_value[word] = 7;
    else if (word.size() == 4) m_word_to_value[word] = 4;
    else if (word.size() == 7) m_word_to_value[word] = 8;
    else m_word_to_value[word] = std::nullopt;
  }

  // Checks if all numbers has been sent. Should always return true.
  [[nodiscard]] bool check_if_all_9() const {
    const auto s = m_word_to_value.size();
    return s == 10;
  }

  // Find the right word for 6 0 9 3 2 5
  void determine() {
    if (!check_if_all_9()) std::cerr << "Error not all numbers have been sent" << '\n';
    
    // Build the mapping from known numbers to letters (ie 1 4 7 8)
    for (const auto & [word, number] : m_word_to_value) {
      if (number) {
        mapping[*number] = word;
      }
    }
    
    //   0:      1:      2:      3:      4:
    //  aaaa    ....    aaaa    aaaa    ....
    // b    c  .    c  .    c  .    c  b    c
    // b    c  .    c  .    c  .    c  b    c
    //  ....    ....    dddd    dddd    dddd
    // e    f  .    f  e    .  .    f  .    f
    // e    f  .    f  e    .  .    f  .    f
    //  gggg    ....    gggg    gggg    ....
    // 
    //   5:      6:      7:      8:      9:
    //  aaaa    aaaa    aaaa    aaaa    aaaa
    // b    .  b    .  .    c  b    c  b    c
    // b    .  b    .  .    c  b    c  b    c
    //  dddd    dddd    ....    dddd    dddd
    // .    f  e    f  .    f  e    f  .    f
    // .    f  e    f  .    f  e    f  .    f
    //  gggg    gggg    ....    gggg    gggg


    // Length 6: we want to identify 6 0 and 9.
    // The only number that do not have all the segments of 1 (cf)   is 6
    // ---------------------------------------------------- 4 (bcdf) are 6 and 0
    // Then we can identify 9
    find_the_mapping_of_length_not_included_in(6, size_t(6), mapping[1]);
    find_the_mapping_of_length_not_included_in(0, size_t(6), mapping[4]);
    find_unmapped(9, size_t(6));

    // Length: 5 -> 2 3 5
    // 3 is the only one to have the full 1 (cf) bar
    find_the_mapping_of_length_that_includes(3, size_t(5), mapping[1]);
    // 2 not included in 9. Notice that here an unknonw number is not included into a known number
    find_the_mapping_of_length_not_included_in(2, size_t(5), mapping[9], true);
    // ok
    find_unmapped(5, size_t(5));
  }

  /**
   * Return the digit that corresponds to the word.
   * 
   * determine() must have been called before
   */
  [[nodiscard]] int get(std::string word) /* const */ {
    std::sort(word.begin(), word.end());
    return m_word_to_value[word].value();
  }

private:
  // Ugly functions that could probably be refactored

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
      line, [](std::string s) { return s; }, ' '
    );

    // ~~ Part A: just find 1 4 7 8. They are trivial as they are the only one
    // with their length.
    for (size_t i = 11 ; i != values.size(); ++i) {
      const auto sz = values[i].size();

      if (sz == 2) ++part_a; // 1
      if (sz == 4) ++part_a; // 4
      if (sz == 3) ++part_a; // 7
      if (sz == 7) ++part_a; // 8
    }

    // ~~ Part B

    // Add every word / set of wire that appears.
    LetterIdentifier number_identifier;

    for (size_t i = 0; i != values.size(); ++i) {
      if (i == 10) continue;
      number_identifier.register_word(values[i]);
    }

    // All number thas been seen. Deduce which numbers are what with some
    // white magic.
    number_identifier.determine();

    // We are supposed to build the number a * 1000 + b * 100 + c * 10 + d.
    // We are too smart for this.
    part_b += number_identifier.get(values[11]) * 1000;
    part_b += number_identifier.get(values[12]) * 100;
    part_b += number_identifier.get(values[13]) * 10;
    part_b += number_identifier.get(values[14]) * 1;
  }

  // Done
  return Output(part_a, part_b);
}
