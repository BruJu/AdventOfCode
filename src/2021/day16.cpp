#include "../advent_of_code.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <array>
#include "../util/position.hpp"

// https://adventofcode.com/2021/day/16

std::vector<bool> to_bits(const std::string & line) {
  std::vector<bool> res;

  for (const char c : line) {
    int value = c >= 'A' && c <= 'F' ? (c - 'A' + 10) : c - '0';

    res.push_back(value & 8);
    res.push_back(value & 4);
    res.push_back(value & 2);
    res.push_back(value & 1);
  }

  return res;
}


int extract_number(const std::vector<bool> & bits, size_t & cursor, size_t length) {
  int value = 0;

  for (size_t i = cursor; i != cursor + length; ++i) {
    value = value * 2;
    if (bits[i]) value += 1;
  }

  cursor += length;

  return value;
}

struct Packet {
  int packet_version;
  int packet_id;

  // if packet_id == 4
  long long int value;

  // if packet_id != 4
  std::vector<Packet> subpackets;

  static Packet build_packet(const std::vector<bool> & bits, size_t & cursor) {
    const auto packet_version = extract_number(bits, cursor, 3);
    const auto packet_id = extract_number(bits, cursor, 3);

    Packet self = Packet { packet_version, packet_id };

    if (packet_id == 4) {
      // literal type
      std::vector<int> contained_values;
      
      while (true) {
        bool has_next = bits[cursor++];

        contained_values.push_back(extract_number(bits, cursor, 4));

        if (!has_next) {
          break;
        }
      }

      long long int real_value = 0;
      for (const auto & small : contained_values) {
        real_value = (real_value << 4) + small;
      }

      self.value = real_value;
    } else {
      const auto length_type_id = extract_number(bits, cursor, 1);

      if (length_type_id == 0) {
        int total_length = extract_number(bits, cursor, 15);

        size_t current_pos = cursor;

        while (cursor < current_pos + total_length) {
          self.subpackets.push_back(Packet::build_packet(bits, cursor));
        }
      } else {
        int nb_of_subpackets = extract_number(bits, cursor, 11);

        for (int i = 0; i != nb_of_subpackets; ++i) {
          self.subpackets.push_back(Packet::build_packet(bits, cursor));
        }
      }
    }

    return self;
  }

  long long int sum_of_versions() const {
    long long int s = packet_version;

    for (const auto & sub : subpackets) {
      s += sub.sum_of_versions();
    }

    return s;
  }

  long long int v() const {
    if (packet_id == 4) return value;

    if (packet_id == 0) {
      long long int s = 0;
      for (const auto & sub : subpackets) {
        s += sub.v();
      }
      return s;

    } else if (packet_id == 1) {

      long long int s = 1;
      for (const auto & sub : subpackets) {
        s *= sub.v();
      }
      return s;

    } else if (packet_id == 2) {
      std::optional<long long int> min;

      for (const auto & sub : subpackets) {
        const auto me = sub.v();
        if (!min || *min > me) min = me;
      }

      return *min;


    } else if (packet_id == 3) {
      std::optional<long long int> max;

      for (const auto & sub : subpackets) {
        const auto me = sub.v();
        if (!max || *max < me) max = me;
      }

      return *max;


    }  else if (packet_id == 5) {

      if (subpackets[0].v() > subpackets[1].v()) return 1;
      else return 0;

    }  else if (packet_id == 6) {

      if (subpackets[0].v() < subpackets[1].v()) return 1;
      else return 0;

    } else {
      return subpackets[0].v() == subpackets[1].v() ? 1 : 0;
    }


  }
};


Output day_2021_16(const std::vector<std::string> & lines, const DayExtraInfo &) {

  std::vector<bool> as_bits = to_bits(lines[0]);

  size_t cursor = 0;

  const auto packet = Packet::build_packet(as_bits, cursor);

  long long int part_a = packet.sum_of_versions();

  
  return Output(part_a, packet.v());
}
