#include "libs.hpp"

#include <algorithm>
#include <vector>
#include <variant>
#include <memory>
#include <set>
#include <map>
#include <regex>
#include <ostream>
#include <cmath>

// https://adventofcode.com/2020/day/20

struct Tile;

class RawPixels {
    std::vector<std::vector<char>> m_data;
public:

    [[nodiscard]] bool connect(const RawPixels & rhs) const {
        for (size_t i = 0 ; i != m_data.size() ; ++i) {
            if (m_data[i].back() != rhs.m_data[i].front()) {
                return false;
            }
        }

        return true;
    }
    
    [[nodiscard]] bool connect_top_to_down(const RawPixels & rhs) const {
        return m_data.back() == rhs.m_data.front();
    }

    [[nodiscard]] bool operator==(const RawPixels & other) const { return m_data == other.m_data; }
    [[nodiscard]] bool operator< (const RawPixels & other) const { return m_data < other.m_data; }


    friend std::ostream & operator<<(std::ostream & stream, const RawPixels & pixels) {
        for (const auto & line : pixels.m_data) {
            for (const auto pixel : line) {
                stream << pixel;
            }
            stream << '\n';
        }

        return stream;
    }

    void flip_left_right() {
        for (auto & line : m_data) {
            std::vector<char> new_line { line.rbegin(), line.rend() };
            line = std::move(new_line);
        }
    }

    void flip_bottom_up() {
        std::vector<std::vector<char>> image { m_data.rbegin(), m_data.rend() };
        m_data = std::move(image);
    }

    void rotate_right() {
        std::vector<std::vector<char>> image;

        for (size_t y = 0 ; y != m_data.size() ; ++y) {
            image.push_back({});
            for (size_t x = 0 ; x != m_data[y].size() ; ++x) {
                image.back().push_back(m_data[m_data.size() - x - 1][y]);
            }
        }

        m_data = std::move(image);
    }

    friend std::vector<Tile> to_tiles(const std::vector<std::string> & lines);

    //const auto & operator[](size_t i) const { return m_data[i]; }


    [[nodiscard]] static RawPixels assemble_mutiple(const std::vector<std::vector<RawPixels>> & raws) {
        RawPixels pixels;

        const size_t tile_size = raws[0][0].m_data.size();

        for (const auto & line_of_raws : raws) {

            const size_t tile_y = pixels.m_data.size();
            for (size_t i = 0 ; i != tile_size - 2 ; ++i) {
                pixels.m_data.push_back({});
            }

            for (const auto & raw : line_of_raws) {

                for (size_t y = 1 ; y != raw.m_data.size() - 1; ++y) {
                    for (size_t x = 1 ; x != raw.m_data[y].size() - 1; ++x) {
                        pixels.m_data[tile_y + y - 1].emplace_back(raw.m_data[y][x]);
                    }
                }
            }
        }

        return pixels;
    }

    // -----------------
    // Monster searching

private:
    static constexpr const char * monster_t = "                  # ";
    static constexpr const char * monster_m = "#    ##    ##    ###";
    static constexpr const char * monster_b = " #  #  #  #  #  #   ";
    static constexpr size_t monster_width = 20;
    static constexpr size_t monster_height = 3;

    bool search_monster_at_and(size_t x, size_t y, bool replace = false) {
        const auto point = [&](char & reality, char monster, bool replace) -> bool {
            if (monster == ' ') {
                return true;
            }

            if (reality == '#' || reality == 'R') {
                if (replace) {
                    reality = 'R';
                }

                return true;
            } else {
                return false;
            }
        };

        for (size_t i = 0 ; i != monster_width ; ++i) {
            if (!point(m_data[y    ][x + i], monster_t[i], replace)) return false;
            if (!point(m_data[y + 1][x + i], monster_m[i], replace)) return false;
            if (!point(m_data[y + 2][x + i], monster_b[i], replace)) return false;
        }

        return true;
    }

    bool search_monster_at(size_t x, size_t y) {
        if (search_monster_at_and(x, y)) {
            search_monster_at_and(x, y, true);
            return true;
        } else {
            return false;
        }
    }
    
   [[nodiscard]] size_t search_monster_() {
        for (size_t x = 0 ; x + monster_width  != m_data.size() ; ++x) {
        for (size_t y = 0 ; y + monster_height != m_data.size() ; ++y) {
            search_monster_at(x, y);
        }}

        size_t rought = 0;
        for (const auto & line : m_data) {
            for (const char c : line) {
                if (c == '#') {
                    rought += 1;
                }
            }
        }

        return rought;
   }


public:
   [[nodiscard]] size_t search_monster() const {
       RawPixels copy = *this;
       return copy.search_monster_();
   }


};


struct Tile {
    int id;
    RawPixels image;

    explicit Tile(int id) : id(id) {}
    explicit Tile(int id, RawPixels image) : id(id), image(std::move(image)) {}

    //[[nodiscard]] const std::vector<char> & operator[](size_t x) const { return image[x]; }

    friend std::ostream & operator<<(std::ostream & stream, const Tile & tile) {
        return stream << "== Tile " << tile.id << '\n' << tile.image << '\n';
    }

    [[nodiscard]] bool adjacent(const Tile & other_tile) const;

    [[nodiscard]] bool adjacent_left_to_right(const Tile & other_tile) const {
        return image.connect(other_tile.image);
    }

    [[nodiscard]] bool adjacent_top_to_down(const Tile & other_tile) const {
        return image.connect_top_to_down(other_tile.image);
    }
};

std::vector<Tile> to_tiles(const std::vector<std::string> & lines) {
    std::vector<Tile> retval;

    for (const auto & line : lines) {
        if (line == "") continue;

        if (line.rfind("Tile ", 0) == 0) {
            std::string id { line.begin() + 5, line.end() - 1 };
            retval.emplace_back(std::stoi(id));
        } else {
            retval.back().image.m_data.emplace_back();

            for (const auto c : line) {
                retval.back().image.m_data.back().emplace_back(c);
            }
        }
    }

    return retval;
}


namespace part_a {

    using AltereredTile = Tile;

    // TODO : A tile can generate a list of tiles it can transform into
    struct TileStateExplorer {
        int i = 0;

        operator bool() {
            return i != 2 * 2 * 4;
        }

        void operator()(Tile & tile) {
            tile.image.rotate_right();

            if (i % 4 == 0) {
                tile.image.flip_left_right();
            }
            if (i % 8 == 0) {
                tile.image.flip_bottom_up();
            }

            ++i;
        }
    };



    struct TempGrid {
        struct TTile {
            const Tile * tile;
            bool used = false;
        };

        std::vector<std::vector<std::optional<AltereredTile>>> m_grid;
        std::vector<TTile> tiles;
        size_t m_size;

        explicit TempGrid(const std::vector<Tile> & p_tiles) {
            for (const auto & tile : p_tiles) {
                tiles.push_back(TTile { &tile, false });
            }

            const size_t size = static_cast<size_t>(std::sqrt(tiles.size()));

            for (size_t i = 0 ; i != size; ++i) {
                m_grid.emplace_back();
                for (size_t j = 0 ; j != size ; ++j) {
                    m_grid.back().emplace_back(std::nullopt);
                }
            }
            m_size = size;
        }

        bool resolve() {
            return try_place_at(0, 0);
        }

        [[nodiscard]] size_t min_required_path(size_t x, size_t y) {
            size_t req = 2;

            if (x != 0 && x != m_size) ++req;
            if (y != 0 && y != m_size) ++req;

            return req;
        }

        bool compatible(size_t x, size_t y) const {
            if (x != 0 && m_grid[y][x - 1] && !m_grid[y][x - 1]->adjacent_left_to_right(*m_grid[y][x])) {
                return false;
            }

            if (y != 0 && m_grid[y - 1][x] && !m_grid[y - 1][x]->adjacent_top_to_down(*m_grid[y][x])) {
                return false;
            }

            if (x != m_size - 1 && m_grid[y][x + 1] && !m_grid[y][x]->adjacent_left_to_right(*m_grid[y][x + 1])) {
                return false;
            }

            if (y != m_size - 1 && m_grid[y + 1][x] && !m_grid[y][x]->adjacent_top_to_down(*m_grid[y + 1][x])) {
                return false;
            }

            return true;
        }

        bool try_place_at(size_t x, size_t y) {
            if (x == m_size * 2 + 1) return true;
            const size_t next_x = x != 0 ? x - 1 : y + 1;
            const size_t next_y = x != 0 ? y + 1 : 0;
            if (x >= m_size || y >= m_size) {
                return try_place_at(next_x, next_y);
            }

            for (auto & [tile, used] : tiles) {
                if (used) continue;

                used = true;

                m_grid[y][x] = AltereredTile { *tile };

                for (TileStateExplorer trans ; trans ; trans(*m_grid[y][x])) {
                    if (!compatible(x, y)) {
                        continue;
                    }

                    if (try_place_at(next_x, next_y)) {
                        return true;
                    }
                }

                m_grid[y][x] = std::nullopt;
                used = false;
            }

            return false;
        }
    };

    struct Grid {
        std::vector<std::vector<int>> m_grid;
        std::vector<std::vector<RawPixels>> m_raws;

        explicit Grid(const TempGrid & temp_grid) {
            for (const auto & line : temp_grid.m_grid) {
                m_grid.emplace_back();
                m_raws.emplace_back();
                for (const auto & value : line) {
                    m_grid.back().emplace_back(value->id);
                    m_raws.back().emplace_back(value->image);
                }
            }
        }

        [[nodiscard]] test::Value corners() const noexcept {
            return test::Value(m_grid.front().front())
                * test::Value(m_grid.front().back())
                * test::Value(m_grid.back().front())
                * test::Value(m_grid.back().back());
        }


        static std::optional<Grid> make(const std::vector<Tile> & tiles) {
            TempGrid temp_grid { tiles };

            if (temp_grid.resolve()) {
                return Grid(temp_grid);
            } else {
                return std::nullopt;
            }
        }
    };

    std::optional<Grid> solve(const std::vector<std::string> & lines) {
        std::vector<Tile> tiles = to_tiles(lines);

        std::map<int, std::set<int>> all_paths;

        for (const auto & tile : tiles) {
            for (const auto & tile_ : tiles) {
                if (&tile != &tile_ && tile.adjacent(tile_)) {
                    all_paths[tile.id].insert(tile_.id);
                }
            }
        }

        // Heuristic : We should start with pieces that are connected to few others
        std::sort(tiles.begin(), tiles.end(), 
            [&](const Tile & lhs, const Tile & rhs) {
                size_t lhs_score = all_paths[lhs.id].size();
                size_t rhs_score = all_paths[rhs.id].size();

                return lhs_score < rhs_score;
            }
        );

        return Grid::make(tiles);
    }
}


bool Tile::adjacent(const Tile & other_tile) const {
    part_a::AltereredTile me { *this };

    std::set<RawPixels> my_raws;

    for (size_t i = 0 ; i != 4 ; ++i, me.image.rotate_right()) {
        if (my_raws.contains(me.image)) {
            continue;
        }

        my_raws.insert(me.image);

        std::set<RawPixels> other_raws;
        part_a::AltereredTile other { other_tile };

        for (part_a::TileStateExplorer trans ; trans ; trans(other)) {
            if (!other_raws.contains(other.image)) {
                other_raws.insert(other.image);

                if (me.image.connect(other.image)) {
                    return true;
                }
            }
        }
    }

    return false;
}


namespace part_b {
    size_t solve(const part_a::Grid & grid) {
        Tile big_picture = Tile(0, RawPixels::assemble_mutiple(grid.m_raws));

        std::optional<size_t> found = std::nullopt;
        for (part_a::TileStateExplorer trans ; trans ; trans(big_picture)) {
            const auto found_monsters = big_picture.image.search_monster();
            found = found ? std::min(*found, found_monsters) : found_monsters;
        }

        return *found;
    }
}

Output day20(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::optional<part_a::Grid> grid = part_a::solve(lines);
    if (!grid) return Output(0, 0);

    const size_t roughness = part_b::solve(*grid);
    return Output(grid->corners(), roughness);
}
