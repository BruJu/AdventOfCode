#include "libs.hpp"
#include <unordered_map>
#include <vector>
#include <set>
#include <map>
#include <regex>
#include <algorithm>

// https://adventofcode.com/2020/day/17

// Hard coding 3D / 4D is too mainstream.
// So here it is : generalization of game of life to any dimensional space.

/// Currently spported bounds for a dimension
struct Dimension {
    size_t offset_of_zero;  // -offset_of_zero == min_length
    size_t max_length;      // exclusive
};

// Supported bounds for each dimension
template <size_t NB_DIM>
struct SpaceSize : public std::array<Dimension, NB_DIM> {  
    [[nodiscard]] SpaceSize next() const noexcept {
        SpaceSize retval = *this;

        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            ++retval[i].offset_of_zero;
            ++retval[i].max_length;
        }

        return retval;
    }

    [[nodiscard]] size_t size() const noexcept {
        size_t n = 1;

        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            n *= (*this)[i].offset_of_zero + (*this)[i].max_length;
        }

        return n;
    }
};

/// A position in the NB_DIM D space
template <size_t NB_DIM>
struct Position {
    std::array<int, NB_DIM> m_coordinate;

    Position() {
        m_coordinate.fill(0);
    }

    Position(size_t pos, const SpaceSize<NB_DIM> & space);

    constexpr bool operator==(const Position & other) {
        return m_coordinate == other.m_coordinate;
    }

    constexpr Position & operator+=(const Position & other) {
        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            m_coordinate[i] += other.m_coordinate[i];
        }
        return *this;
    }

    [[nodiscard]] constexpr Position operator+(const Position & other) const {
        Position copy = *this;
        copy += other;
        return copy;
    }

    [[nodiscard]] constexpr size_t to_array_position(SpaceSize<NB_DIM> space_size) const {
        size_t my_position = 0;

        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            int in_my_dimension = m_coordinate[i] + space_size[i].offset_of_zero;
            my_position = my_position * (space_size[i].offset_of_zero + space_size[i].max_length) + in_my_dimension;
        }

        return my_position;
    }

    [[nodiscard]] constexpr std::optional<size_t> to_array_position_maybe(SpaceSize<NB_DIM> space_size) const {
        size_t my_position = 0;

        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            if (m_coordinate[i] < 0 && static_cast<size_t>(-m_coordinate[i]) > space_size[i].offset_of_zero) return std::nullopt;
            if (m_coordinate[i] > 0 && static_cast<size_t>(m_coordinate[i]) >= space_size[i].max_length) return std::nullopt;

            int in_my_dimension = m_coordinate[i] + space_size[i].offset_of_zero;
            my_position = my_position * (space_size[i].offset_of_zero + space_size[i].max_length) + in_my_dimension;
        }

        return my_position;
    }

    friend std::ostream & operator<<(std::ostream & stream, const Position<NB_DIM> & position) {
        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            stream << position.m_coordinate[i] << ' ';
        }
        return stream;
    }

    template <typename Consumer>
    void for_each_neighbour_position(Consumer consumer) const {
        Position offset;
        offset.m_coordinate.fill(-1);

        size_t i = 0;
        while (true) {
            if (i != NB_DIM) {
                // On a non 1 coordinate, go to next coordinate
                ++i;
            } else {
                // We have a new neighbour coordinate, apply consumer
                if (std::any_of(offset.m_coordinate.begin(), offset.m_coordinate.end(), [](int x) { return x != 0; })) {
                    consumer(*this + offset);
                }

                // Go back to last non 1 coordinate
                while (true) {
                    if (i == 0) {
                        // offset was 1 (...) 1
                        return;
                    }

                    --i;
                    
                    if (offset.m_coordinate[i] == 1) {
                        offset.m_coordinate[i] = -1; // Reset
                        // go back
                    } else /* if (offset.m_coordinate[i] != 1) */ {
                        ++offset.m_coordinate[i];
                        break;
                    }
                }
            }
        }
    }
};

template <size_t NB_DIM>
Position<NB_DIM>::Position(size_t pos, const SpaceSize<NB_DIM> & space) {
    for (size_t i = 0 ; i != NB_DIM ; ++i) {
        size_t real_i = NB_DIM - 1 - i;
        size_t dim_size = space[real_i].offset_of_zero + space[real_i].max_length;
        m_coordinate[real_i]  = pos % dim_size;
        m_coordinate[real_i] -= space[real_i].offset_of_zero;

        pos = pos / dim_size;
    }
}

/// A NB_DIM world
template <size_t NB_DIM>
struct Field {
    std::vector<char> m_symbols;
    SpaceSize<NB_DIM> m_spacesize;

    Field(const std::vector<std::string> & lines);

    void next();

    [[nodiscard]] test::Value count_occupied() const noexcept {
        return std::count_if(m_symbols.begin(), m_symbols.end(), 
            [](char c) { return c == '#'; }
        );
    }

    char get_at(Position<NB_DIM> position) {
        const auto maybe = position.to_array_position_maybe(m_spacesize);
        if (!maybe) return '.';

        return m_symbols[*maybe];
    }
};

template <size_t NB_DIM>
Field<NB_DIM>::Field(const std::vector<std::string> & lines) {
    m_spacesize.fill(Dimension{ 0, 1 });
    m_spacesize[0] = Dimension{ 0, lines[0].size() };
    m_spacesize[1] = Dimension{ 0, lines.size() };

    for (size_t y = 0 ; y != lines.size() ; ++y) {
        for (size_t x = 0 ; x != lines[y].size() ; ++x) {
            // Position<NB_DIM> position;
            // position.m_coordinate[1] = x;
            // position.m_coordinate[0] = y;
            // const size_t pos = position.to_array_position(m_spacesize);
            // assert(pos == m_symboles.size());
            m_symbols.push_back(lines[y][x]);
        }
    }
}

template <size_t NB_DIM>
void Field<NB_DIM>::next() {
    std::vector<char> next_symbols;
    SpaceSize<NB_DIM> next_spacesize = m_spacesize.next();

    const size_t size = next_spacesize.size();
    for (size_t i = 0 ; i != size ; ++i) {
        Position p = Position(i, next_spacesize);
        char my_position = get_at(p);

        size_t occ = 0;

        p.for_each_neighbour_position(
            [&](Position<NB_DIM> neighbour) {
                if (get_at(neighbour) == '#') {
                    occ += 1;
                }
            }
        );

        if (my_position == '#') {
            next_symbols.push_back((occ == 2 || occ == 3) ? '#' : '.');
        } else {
            next_symbols.push_back(             occ == 3  ? '#' : '.');
        }
    }

    m_symbols = next_symbols;
    m_spacesize = next_spacesize;
}

/// What the exercice was again?
template<size_t NB_DIM>
static test::Value occupied_after_six_iterations(const std::vector<std::string> & lines) {
    Field<NB_DIM> field { lines };

    for (size_t i = 0 ; i != 6 ; ++i) field.next();

    return field.count_occupied();
}

/// 3D / 4D game of life
Output day17(const std::vector<std::string> & lines, const DayExtraInfo &) {
    return Output(
        occupied_after_six_iterations<3>(lines),
        occupied_after_six_iterations<4>(lines)
    );
}
