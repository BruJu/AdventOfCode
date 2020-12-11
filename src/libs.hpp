#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <iostream>

#include <fstream>
#include <sstream>

struct Output {
    using Type = long long int;
    using Optional = std::optional<Type>;
    
    Optional part1 = std::nullopt;
    Optional part2 = std::nullopt;
};

class StringSplitter {
    std::string_view::const_iterator pos;
    std::string_view::const_iterator end;

public:
    explicit StringSplitter(const std::string_view & str) : pos(str.begin()), end(str.end()) {}

    std::string operator()() {
        const auto from = pos;

        while (pos != end && *pos != ' ') {
            ++pos;
        }

        const auto to = pos;

        while (pos != end && *pos == ' ') {
            ++pos;
        }

        return std::string(from, to);
    }
};

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

