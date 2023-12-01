#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cstdint>
#include <iostream>

namespace bj {
  namespace nested_ints {
    struct NestedInts {
      bool is_integer;
      std::int64_t direct_int;
      std::vector<NestedInts> nested;

      explicit NestedInts(std::int64_t direct_int) : is_integer(true), direct_int(direct_int) {}
      explicit NestedInts(std::vector<NestedInts> nested) : is_integer(false), direct_int(0), nested(std::move(nested)) {}

      [[nodiscard]] bool operator==(const NestedInts & other) const {
        if (is_integer != other.is_integer) return false;
        if (is_integer) {
          return direct_int == other.direct_int;
        } else {
          if (nested.size() != other.nested.size()) return false;
          for (size_t i = 0; i != nested.size(); ++i) {
            if (nested[i] != other.nested[i]) return false;
          }
          return true;
        }
      }

      [[nodiscard]]bool operator!=(const NestedInts & other) const {
        return !(operator==(other));
      }

      friend std::ostream & operator<<(std::ostream & stream, const NestedInts & self) {
        if (self.is_integer) {
          return stream << self.direct_int;
        } else {
          stream << '[';
          for (size_t i = 0; i != self.nested.size(); ++i) {
            if (i != 0) stream << ',';
            stream << self.nested[i];
          }
          return stream << ']';
        }
      }
    };

    struct Reader {
      std::string_view view;
      size_t i = 0;

      Reader(std::string_view view) : view(view) {}

      [[nodiscard]] NestedInts next() {
        if (view[i] >= '0' && view[i] <= '9') {
          std::int64_t x = 0;

          while (i <= view.size() && view[i] != ']' && view[i] != ',') {
            x += x * 10 + (view[i] - '0');
            ++i;
          }

          return NestedInts(x);
        } else if (view[i] == '[') {
          std::vector<NestedInts> values;

          ++i; // consume [

          while (view[i] != ']') {
            values.emplace_back(next());

            if (view[i] == ',') ++i;
          }

          ++i;  // consume ]
          return NestedInts(values);
        } else {
          std::cerr << i << " " << view << "\n";
          throw std::runtime_error("Bad string passed to nested_ints");
        }
      }
    };

    inline NestedInts read(const std::string & line) {
      if (line.size() == 0) return NestedInts(0);
      Reader reader(line);
      return reader.next();
    }
  }

}
