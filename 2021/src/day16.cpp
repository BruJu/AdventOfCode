#include "../../common/advent_of_code.hpp"
#include <vector>
#include <optional>

// https://adventofcode.com/2021/day/16
// Rank 245 ... and I started late
// (I already did packet / binary decoding 3 times, and all these times
// I didn't have access to a proper complete documentation)

/** Converts an hexadecimal-format string into a sequence of bits */
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

/** Extract the number encoded at bits[cursor:cursor + length] */
static int extract_number(const std::vector<bool> & bits, size_t & cursor, size_t length) {
  int value = 0;

  for (size_t i = cursor; i != cursor + length; ++i) {
    value = value * 2;
    if (bits[i]) value += 1;
  }

  cursor += length;

  return value;
}

/** Packet Id Semantic */
enum class PacketId : int {
  Sum = 0,
  Product = 1,
  Min = 2,
  Max = 3,
  Literal = 4,
  Greater = 5,
  Lesser = 6,
  Equal = 7
};

struct Packet {
  int packet_version;
  PacketId packet_id;

  // if packet_id == 4
  long long int literal_value;

  // if packet_id != 4
  std::vector<Packet> subpackets;

  /** Constructor for an empty packet */
  Packet(int packet_version, PacketId packet_id)
  : packet_version(packet_version), packet_id(packet_id), literal_value(0) {}

  /** Builds a packet from a sequence of bit starting from a given position */
  static Packet build_packet(const std::vector<bool> & bits, size_t & cursor) {
    const auto packet_version = extract_number(bits, cursor, 3);
    const auto packet_id = static_cast<PacketId>(extract_number(bits, cursor, 3));

    Packet self = Packet(packet_version, packet_id);

    if (packet_id == PacketId::Literal) {
      while (true) {
        const bool has_next = bits[cursor++];

        self.literal_value <<= 4;
        self.literal_value += extract_number(bits, cursor, 4);

        if (!has_next) {
          break;
        }
      }
    } else {
      const auto length_type_id = extract_number(bits, cursor, 1);

      if (length_type_id == 0) {
        const int total_length = extract_number(bits, cursor, 15);

        const size_t current_pos = cursor;

        while (cursor < current_pos + total_length) {
          self.subpackets.push_back(Packet::build_packet(bits, cursor));
        }
      } else {
        const int nb_of_subpackets = extract_number(bits, cursor, 11);

        for (int i = 0; i != nb_of_subpackets; ++i) {
          self.subpackets.push_back(Packet::build_packet(bits, cursor));
        }
      }
    }

    return self;
  }

  [[nodiscard]] long long int sum_of_versions() const noexcept {
    long long int s = packet_version;

    for (const auto & sub : subpackets) {
      s += sub.sum_of_versions();
    }

    return s;
  }

  [[nodiscard]] long long int v() const {
    switch (packet_id) {
      case PacketId::Sum: {
        long long int acc = 0;

        for (const auto & sub : subpackets) {
          acc += sub.v();
        }

        return acc;
      }
      case PacketId::Product: {
        long long int acc = 1;

        for (const auto & sub : subpackets) {
          acc *= sub.v();
        }

        return acc;
      }
      case PacketId::Min: {
        std::optional<long long int> min;

        for (const auto & sub : subpackets) {
          const auto me = sub.v();
          if (!min || *min > me) min = me;
        }

        return *min;
      }
      case PacketId::Max: {
        std::optional<long long int> max;

        for (const auto & sub : subpackets) {
          const auto me = sub.v();
          if (!max || *max < me) max = me;
        }

        return *max;
      }
      case PacketId::Literal:
        return literal_value;
      case PacketId::Greater:
        return (subpackets[0].v() > subpackets[1].v()) ? 1 : 0;
      case PacketId::Lesser:
        return (subpackets[0].v() < subpackets[1].v()) ? 1 : 0;
      case PacketId::Equal:
        return (subpackets[0].v() == subpackets[1].v()) ? 1 : 0;
      default: 
        std::cerr << "2021-16: Unknown operator " << static_cast<int>(packet_id) << '\n';
        return 0;
    }
  }
};


Output day_2021_16(const std::vector<std::string> & lines, const DayExtraInfo &) {
  // More accurate input
  const std::vector<bool> as_bits = to_bits(lines[0]);

  // Make a packet
  size_t cursor = 0;
  const auto packet = Packet::build_packet(as_bits, cursor);

  // Done
  return Output(packet.sum_of_versions(), packet.v());
}
