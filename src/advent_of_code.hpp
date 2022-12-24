#pragma once

#include <array>
#include <variant>
#include <vector>
#include <span>
#include <string>
#include <string_view>
#include <optional>
#include <iostream>
#include <chrono>

#include <fstream>
#include <sstream>

#include "colors.h"


namespace test {
    //using Value = long long int;

    struct Expected {
        enum class Type { /* _ */ Ignore, /* 123 */ Known, /* ? */ Wanted, /* ?123 */ WantedKnown, Inline };

        Type        type;
        std::string value;
        int extra_parameter = 0;

        explicit Expected(std::string line);
        explicit Expected(Type type) : type(type), value("") {}

        friend std::ostream & operator<<(std::ostream & stream, const Expected & expected) {
            if (expected.type == Type::Ignore) {
                stream << 'X';
            } else if (expected.type == Type::Known) {
                stream << "Expect" << expected.value;
            } else if (expected.type == Type::WantedKnown) {
                stream << "?" << expected.value;
            } else {
                stream << "?";
            }

            return stream;
        }

        [[nodiscard]] bool is_ignore() const noexcept { return type == Type::Ignore; }
    };
    
    enum class TestValidation { Success, Fail, Computed };

    inline TestValidation validation_reduce(std::optional<TestValidation> lhs, std::optional<TestValidation> rhs) {
        if (!lhs && !rhs) return TestValidation::Computed;
        if ( lhs && !rhs) return *lhs;
        if (!lhs &&  rhs) return *rhs;
        if (*lhs == TestValidation::Fail || *rhs == TestValidation::Fail) return TestValidation::Fail;

        return *lhs;
    }

    struct PartResult {
        TestValidation type; // Ignore, Known or Wanted
        std::string computed;
        std::string expected;

        [[nodiscard]] bool is_valid() const noexcept { return computed == expected; }
        
        static std::optional<PartResult> from(const std::string & computed, const test::Expected & expected) {
            PartResult p;
            p.computed = computed;
            p.expected = expected.value;

            switch (expected.type) {
                case Expected::Type::Ignore: 
                case Expected::Type::Inline:
                    return std::nullopt;
                case Expected::Type::Known:
                    p.type = p.computed == p.expected ? TestValidation::Success : TestValidation::Fail;
                    break;
                case Expected::Type::WantedKnown:
                    p.type = p.computed == p.expected ? TestValidation::Computed : TestValidation::Fail;
                    break;
                case Expected::Type::Wanted:
                    p.type = TestValidation::Computed;
                    break;
            }

            return p;
        }

        static std::optional<PartResult> from(unsigned int valid, unsigned int total, const std::vector<std::string> & extra_message) {
            PartResult p;
            char buffer[256];
            std::sprintf(buffer, "Infile unit tests: %u / %u", valid, total);
            p.computed = buffer;

            if (total == 0) {
                return std::nullopt;
            } else if (valid == total) {
                p.expected = buffer;
                p.type = TestValidation::Success;
            } else {
                // At this point, we don't care about design, we just want to debug easily.
                p.expected = "";
                
                for (const auto & message : extra_message) {
                    std::cerr << message << '\n';
                }

                p.type = TestValidation::Fail;
            }
            
            return p;
        }
    };

    inline const char * get_color(std::optional<TestValidation> test_validation) {
        if (!test_validation) return KCYN;

        switch (*test_validation) {
            case TestValidation::Success : return KGRN;
            case TestValidation::Fail    : return KRED;
            case TestValidation::Computed: return KBLU;
            default:                       return KCYN;
        }
    }

    struct RunResult {
        std::array<std::optional<PartResult>, 2> parts;
        std::chrono::duration<double> elapsed_time;

        [[nodiscard]] TestValidation get_overall() const noexcept;
    };

    inline TestValidation RunResult::get_overall() const noexcept {
        const std::optional<TestValidation> a = parts[0] ? std::optional<TestValidation>(parts[0]->type) : std::nullopt;
        const std::optional<TestValidation> b = parts[1] ? std::optional<TestValidation>(parts[1]->type) : std::nullopt;
        return validation_reduce(a, b);
    }
    
    struct Score {
        unsigned int success = 0;
        unsigned int failed  = 0;
        std::chrono::duration<double> total_time;

        Score & operator+=(const std::optional<RunResult> & run_result) {
            if (!run_result) return *this;

            for (const auto & part_result : run_result->parts) {
                if (part_result) {
                    *this += part_result->type;
                }
            }

            total_time += run_result->elapsed_time;

            return *this;
        }

        Score & operator+=(TestValidation test_validation) {
            if (test_validation == TestValidation::Success) {
                ++success;
            } else if (test_validation == TestValidation::Fail) {
                ++failed;
            }

            return *this;
        }


        [[nodiscard]] unsigned int total() const noexcept { return success + failed; }
    };
}

struct Output {
    std::string part_a;
    std::string part_b;
    
    Output(long long int a, long long int b) : part_a(std::to_string(a)), part_b(std::to_string(b)) {}
    Output(long long int a, std::string b  ) : part_a(std::to_string(a)), part_b(std::move(b))      {}
    Output(std::string a  , long long int b) : part_a(std::move(a))     , part_b(std::to_string(b)) {}
    Output(std::string a  , std::string b  ) : part_a(std::move(a))     , part_b(std::move(b))      {}
};

struct DayExtraInfo {
    bool can_skip_part_A = false;
    int  part_a_extra_param = 0;
    bool can_skip_part_B = false;
    int  part_b_extra_param = 0;
};

class StringSplitter {
    std::string_view::const_iterator pos;
    std::string_view::const_iterator end;
    char split_character;

public:
    explicit StringSplitter(const std::string_view & str, char split_character = ' ')
    : pos(str.begin()), end(str.end()), split_character(split_character) {}

    std::string operator()() {
        const auto from = pos;

        while (pos != end && *pos != split_character) {
            ++pos;
        }

        const auto to = pos;

        while (pos != end && *pos == split_character) {
            ++pos;
        }

        return std::string(from, to);
    }

    [[nodiscard]] operator bool() const { return pos != end; }

    template<typename T, typename Mapper>
    static std::vector<T> line_to_vector(std::string_view line, Mapper mapper, char separator = ' ') {
        std::vector<T> retval;
        StringSplitter splitter = StringSplitter(line, separator);

        while (splitter) {
            retval.emplace_back(mapper(splitter()));
        }

        return retval;
    }
};


namespace bj {
/** string_split(line, separator) is equivalent to line.split(separator) in JavaScript */
inline std::vector<std::string> string_split(
    const std::string_view line,
    const std::string_view separator
) {
  std::vector<std::string> retval;

  size_t segment_start = 0;
  size_t i = 0;
  while (i < line.size()) {
    if (i + separator.size() < line.size()) {
      if (line.substr(i, separator.size()) == separator) {
        retval.emplace_back(line.substr(segment_start, i - segment_start));
        i += separator.size();
        segment_start = i;
        continue;
      }
    }

    ++i;
  }

  retval.emplace_back(line.substr(segment_start, i - segment_start));

  return retval;
}

inline std::vector<int> extract_ints_from_line(const std::string & line) {
  size_t i = 0;

  std::vector<int> values;

  while (true) {
    int accumulator = 0;
    
    while (i < line.size() && !(line[i] >= '0' && line[i] <= '9')) {
      ++i;
    }

    if (i == line.size()) return values;

    while (i < line.size() && (line[i] >= '0' && line[i] <= '9')) {
      accumulator = accumulator * 10 + (line[i] - '0');
      ++i;
    }

    values.emplace_back(accumulator);
  }
}
}

struct Board {
    std::vector<std::vector<char>> m_board;

public:
    explicit Board(const std::vector<std::string> & lines) {
        for (const auto & line : lines) {
            m_board.emplace_back(line.begin(), line.end());
        }
    }

    [[nodiscard]] size_t height() const { return m_board.size(); }

    // Board should not be empty
    [[nodiscard]] size_t width() const { return m_board[0].size(); }

    [[nodiscard]] std::optional<char> get_at(size_t x, size_t y) const {
        if (x >= width()) return std::nullopt;
        if (y >= height()) return std::nullopt;
        return m_board[y][x];
    }

    void set_at(size_t x, size_t y, char c) {
        m_board[y][x] = c;
    }

    [[nodiscard]] int normalize_x(int x) const { return x % width();  }
    [[nodiscard]] int normalize_y(int y) const { return y % height(); }

    [[nodiscard]] bool operator==(const Board & other) const {
        return m_board == other.m_board;
    }
};

class BoardPosition {
    size_t m_x;       bool loop_x;
    size_t m_y;       bool loop_y;
    Board * board;

    static void offset_helper(size_t & i, int offset, size_t length, bool is_looped) {
        if (!is_looped) {
            i += offset;
            return;
        }

        int i_as_int = (static_cast<int>(i) + offset) % length;
        i = static_cast<size_t>(i_as_int);
    }

public:
    BoardPosition(Board & pBoard, size_t x = 0, size_t y = 0, bool pLoop_x = false, bool pLoop_y = false)
    : m_x(x), loop_x(pLoop_x), m_y(y), loop_y(pLoop_y), board(&pBoard) {}

    [[nodiscard]] std::optional<char> operator*() const {
        return board->get_at(m_x, m_y);
    }

    void set(char c) {
        board->set_at(m_x, m_y, c);
    }

    void offset(int x, int y) {
        offset_helper(m_x, x, board->width() , loop_x);
        offset_helper(m_y, y, board->height(), loop_y);
    }
};

template <typename Ts, typename Streamer>
std::ostream & into_stream(std::ostream & stream, const Ts & values, const char * separator, Streamer streamer) {
    bool first = true;

    for (const auto & value : values) {
        if (!first) stream << separator;
        first = false;

        streamer(stream, value);
    }

    return stream;
}

namespace lines_transform {
    inline std::vector<int> to_ints(const std::vector<std::string> & lines) {
        std::vector<int> values;

        for (const std::string & line : lines) {
            values.emplace_back(std::stoi(line));
        }

        return values;
    }

    template <typename T, typename Mapper>
    std::vector<T> map(const std::vector<std::string> & lines, Mapper mapper) {
        std::vector<T> retval;

        for (const std::string & line : lines) {
            retval.emplace_back(mapper(line));
        }

        return retval;
    }

    template <typename D, typename I, typename Mapper, typename Reducer, typename Finalize>
    std::vector<D> group(const std::vector<std::string> & lines, Mapper mapper, Reducer reducer, Finalize finalizer) {
        std::vector<D> values;

        std::optional<I> accumulator = std::nullopt;

        const auto append = [&]() {
            if (accumulator) {
                values.emplace_back(finalizer(accumulator.value()));
                accumulator = std::nullopt;
            }
        };

        for (const std::string & line : lines) {
            if (line == "") {
                append();
            } else if (!accumulator.has_value()) {
                accumulator = mapper(line);
            } else {
                accumulator = reducer(accumulator.value(), mapper(line));
            }
        }

        append();

        return values;
    }

    template <typename T> inline T map_identity(T t) { return t; }
}


template<typename T, size_t N>
struct bad_static_vector {
private:
  std::array<T, N> values {};
  size_t size_ = 0;

public:
  void add(T value) {
    if (size_ == N) throw std::runtime_error("too much elemensts in bad static vector");
    values[size_] = value;
    ++size_;
  }

  [[nodiscard]] std::span<const T> get_all() const {
    return std::span(values.begin(), values.begin() + size_);
  }
  
  [[nodiscard]] std::span<T> get_all() {
    return std::span(values.begin(), values.begin() + size_);
  }

  [[nodiscard]] size_t size() const noexcept { return size_; }

  [[nodiscard]] const T & operator[](const size_t index) const {
    return values[index];
  }
};


