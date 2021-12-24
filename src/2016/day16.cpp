#include "../advent_of_code.hpp"
#include "../util/instruction_reader.hpp"
#include "../util/position.hpp"
#include <cstring>
#include <algorithm>
#include <optional>
#include <array>
#include <set>
#include <map>
#include <queue>

struct Dragon {
  std::vector<bool> m_data;

  explicit Dragon(const std::string & string) {
    for (const char c : string) {
      m_data.push_back(c == '1');
    }
  }

  void dragonify() noexcept {
    size_t it = 0;
    size_t end = m_data.size();

    m_data.push_back(false);
    while (it != end) {
      m_data.push_back(!m_data[end - it - 1]);
      ++it;
    }
  }

  [[nodiscard]] Dragon checksum(size_t length) const noexcept {
    Dragon copy = *this;
    while (copy.m_data.size() > length) {
      copy.m_data.pop_back();
    }

    while (copy.m_data.size() % 2 == 0) {
      size_t i_write = 0;
      size_t i_read = 0;
      const size_t end_read = copy.size();

      while (i_read != end_read) {
        copy.m_data[i_write] = copy.m_data[i_read] == copy.m_data[i_read + 1];
        i_read  += 2;
        i_write += 1;
      }

      while (copy.m_data.size() > i_write) { copy.m_data.pop_back(); }
    }

    return copy;
  }

  [[nodiscard]] std::string to_string() const noexcept {
    std::string s(m_data.size(), '0');

    for (size_t i = 0; i != m_data.size(); ++i) {
      if (m_data[i]) {
        s[i] = '1';
      }
    }

    return s;
  }

  [[nodiscard]] size_t size() const noexcept {
    return m_data.size();
  }
};

Output day_2016_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  const auto space = lines[0].find(' ');
  Dragon dragon(lines[0].substr(0, space));

  const auto wanted_length =
    space != std::string::npos
      ? std::stoul(lines[0].substr(space + 1, std::string::npos))
      : 272;

  while (dragon.size() < wanted_length) {
    dragon.dragonify();
  }

  const auto part_a = dragon.checksum(wanted_length).to_string();

  std::string part_b = "";
  if (wanted_length == 272) {
    while (dragon.size() < 35651584) {
      dragon.dragonify();
    }

    part_b = dragon.checksum(35651584).to_string();
  }

  return Output(part_a, part_b);
}
