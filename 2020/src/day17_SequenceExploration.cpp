#include "../../common/advent_of_code.hpp"

#include <algorithm>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <vector>
#include <functional>


// https://adventofcode.com/2020/day/17

/// A position in the NB_DIM D space
template <size_t NB_DIM>
struct Position {
    std::array<int, NB_DIM> m_coordinate;

    Position() { m_coordinate.fill(0); }

    Position(int x, int y) /* requires NB_DIM >= 2 */ {
        m_coordinate.fill(0);
        m_coordinate[1] = x;
        m_coordinate[0] = y;
    }

    [[nodiscard]] constexpr bool operator<(const Position & other) const {
        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            if (m_coordinate[i] < other.m_coordinate[i]) return true;
            if (m_coordinate[i] > other.m_coordinate[i]) return false;
        }

        return false;
    }

    [[nodiscard]] constexpr bool operator==(const Position & other) const {
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

    friend std::ostream & operator<<(std::ostream & stream, const Position<NB_DIM> & position) {
        stream << '(';
        for (size_t i = 0 ; i != NB_DIM ; ++i) {
            if (i != 0) stream << ", ";
            stream << position.m_coordinate[i];
        }
        stream << ')';
        return stream;
    }

    template <typename Consumer>
    void for_each_neighbour_position(Consumer consumer) const {
        // Instead of searching recursively in a tree structure, we could reuse
        // vector_implementation::SpaceSize to have a mapping between 0 -> 3^NB_DIM -1 values
        // and offset positions

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

namespace std {
    template <size_t NB_DIM> struct hash<Position<NB_DIM>> {
        std::size_t operator()(const Position<NB_DIM> & pos) const noexcept {
            size_t h = 0;
            for (size_t i = 0 ; i != NB_DIM ; ++i) {
                h = h * 31 + std::hash<int>()(pos.m_coordinate[i]);
            }
            return h;
        }
    };
}

namespace vector_implementation {
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

        [[nodiscard]] Position<NB_DIM> to_position(size_t array_index) const {
            Position<NB_DIM> position;

            for (size_t i = 0 ; i != NB_DIM ; ++i) {
                size_t real_i = NB_DIM - 1 - i;
                size_t dim_size = (*this)[real_i].offset_of_zero + (*this)[real_i].max_length;
                position.m_coordinate[real_i]  = array_index % dim_size;
                position.m_coordinate[real_i] -= (*this)[real_i].offset_of_zero;

                array_index = array_index / dim_size;
            }

            return position;
        }

        [[nodiscard]] constexpr size_t to_array_index(const Position<NB_DIM> & position) const {
            return to_array_index_maybe(position).value();
        }

        [[nodiscard]] constexpr std::optional<size_t> to_array_index_maybe(const Position<NB_DIM> & position) const {
            size_t index = 0;

            const auto & coordinate = position.m_coordinate;
    
            for (size_t i = 0 ; i != NB_DIM ; ++i) {
                const Dimension & dim = (*this)[i];

                if (coordinate[i] < 0 && static_cast<size_t>(-coordinate[i]) >  dim.offset_of_zero) return std::nullopt;
                if (coordinate[i] > 0 && static_cast<size_t>( coordinate[i]) >= dim.max_length) return std::nullopt;
    
                int in_my_dimension = coordinate[i] + dim.offset_of_zero;
                index = index * (dim.offset_of_zero + dim.max_length) + in_my_dimension;
            }
    
            return index;
        }
    };

    /// A NB_DIM world
    template <size_t NB_DIM>
    struct Field {
        std::vector<char> m_symbols;
        SpaceSize<NB_DIM> m_spacesize;

        Field(const std::vector<std::string> & lines);

        void next();

        [[nodiscard]] auto count_occupied() const noexcept {
            return std::count_if(m_symbols.begin(), m_symbols.end(), 
                [](char c) { return c == '#'; }
            );
        }

        char get_at(const Position<NB_DIM> & position) {
            const auto maybe = m_spacesize.to_array_index_maybe(position);
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
        const SpaceSize<NB_DIM> next_spacesize = m_spacesize.next();

        const size_t size = next_spacesize.size();
        for (size_t i = 0 ; i != size ; ++i) {
            const Position p = next_spacesize.to_position(i);
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
}

namespace map_implementation {
    template <size_t NB_DIM>
    class Field {
        // using Set = std::set<Position<NB_DIM>>;
        // using Map = std::map<Position<NB_DIM>, size_t>;

        using Set = std::unordered_set<Position<NB_DIM>>;
        using Map = std::unordered_map<Position<NB_DIM>, size_t>;

        Set occupied_slots;

    public:
        explicit Field(const std::vector<std::string> & lines) {
            for (size_t y = 0 ; y != lines.size() ; ++y) {
                for (size_t x = 0 ; x != lines[y].size() ; ++x) {
                    if (lines[y][x] == '#') {
                        occupied_slots.insert(Position<NB_DIM>(x, y));
                    }
                }
            }
        }

        [[nodiscard]] auto count_occupied() const noexcept {
            return occupied_slots.size();
        }

        void next() {
            Map activated_neighbours;

            for (const auto & occupied_position : occupied_slots) {
                occupied_position.for_each_neighbour_position(
                    [&](Position<NB_DIM> neighbour) {
                        ++activated_neighbours[neighbour];
                    }
                );
            }

            Set new_occupation;

            for (const auto & [position, occupied_neighbours] : activated_neighbours) {
                if (occupied_slots.contains(position)) {
                    if (occupied_neighbours == 2 || occupied_neighbours == 3) {
                        new_occupation.insert(position);
                    }
                } else {
                    if (                            occupied_neighbours == 3) {
                        new_occupation.insert(position);
                    }
                }
            }

            occupied_slots = std::move(new_occupation);
        }
    };
}

template<typename FieldClass>
static auto occupied_after_six_iterations(const std::vector<std::string> & lines) {
    FieldClass field { lines };

    for (size_t i = 0 ; i != 6 ; ++i) field.next();

    return field.count_occupied();
}

/// 3D / 4D game of life
Output day_2020_17(const std::vector<std::string> & lines, const DayExtraInfo &) {
    return Output(
        occupied_after_six_iterations<map_implementation::Field<3>>(lines),
        occupied_after_six_iterations<map_implementation::Field<4>>(lines)
    );
}
