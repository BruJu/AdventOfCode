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

using Raw = std::vector<std::vector<char>>;

struct Transformations {
    bool is_flipped_left_right = false;
    bool is_flipped_bottom_up = false;
    int is_rotate_right = 0;

    friend std::ostream & operator<<(std::ostream & stream, const Transformations & trans) {
        stream << "FlippedRL=" << trans.is_flipped_left_right
            << " ; FlippedTB=" << trans.is_flipped_bottom_up
            << " ; rotate= " << trans.is_rotate_right;
        return stream;
    }
};

struct Path;

struct Tile {
    int id;
    std::vector<std::vector<char>> image;

    explicit Tile(int id) : id(id) {}

    [[nodiscard]] const std::vector<char> & operator[](size_t x) const { return image[x]; }

    friend std::ostream & operator<<(std::ostream & stream, const Tile & tile) {
        stream << "== Tile " << tile.id << "\n";

        for (const auto & line : tile.image) {
            for (const auto pixel : line) {
                stream << pixel;
            }

            stream << '\n';
        }

        stream << '\n';
        return stream;
    }

    [[nodiscard]] std::vector<Path> adjacent(const Tile & other_tile) const;
};

struct Path {
    int my_rotation;
    Transformations his_transformations;

    friend std::ostream & operator<<(std::ostream & stream, const Path & path) {
        stream << "left= " << path.my_rotation << " ; " << path.his_transformations;
        return stream;
    }
};

using Paths = std::vector<Path>;
using AllPaths = std::map<int, std::map<int, Paths>>;

struct AltereredTile {
    Tile representation;
    Transformations transformations;

    AltereredTile(Tile tile) : representation(std::move(tile)) {}

    // ok
    void flip_left_right() {
        transformations.is_flipped_left_right = !transformations.is_flipped_left_right;

        for (auto & line : representation.image) {
            std::vector<char> new_line { line.rbegin(), line.rend() };
            line = std::move(new_line);
        }
    }

    // ok
    void flip_bottom_up() {
        transformations.is_flipped_bottom_up = !transformations.is_flipped_bottom_up;

        std::vector<std::vector<char>> image { representation.image.rbegin(), representation.image.rend() };
        representation.image = std::move(image);
    }

    // ok
    void rotate_right() {
        transformations.is_rotate_right = (transformations.is_rotate_right + 1) % 4;

        std::vector<std::vector<char>> image;

        for (size_t y = 0 ; y != representation.image.size() ; ++y) {
            image.push_back({});
            for (size_t x = 0 ; x != representation.image[y].size() ; ++x) {
                image.back().push_back(representation[representation.image.size() - x - 1][y]);
            }
        }

        representation.image = std::move(image);
    }

    friend std::ostream & operator<<(std::ostream & stream, const AltereredTile & tile) {
        stream << tile.representation << tile.transformations << "\n\n";
        return stream;
    }

    [[nodiscard]] bool adjacent_left_to_right(const AltereredTile & other_tile) const;
    [[nodiscard]] bool adjacent_top_to_down(const AltereredTile & other_tile) const;
};

struct TileStateExplorer {
    int i = 0;

    operator bool() {
        return i != 2 * 2 * 4;
    }

    void operator()(AltereredTile & tile) {
        tile.rotate_right();

        if (i % 4 == 0) {
            tile.flip_left_right();
        }
        if (i % 8 == 0) {
            tile.flip_bottom_up();
        }

        ++i;
    }
};


static bool connect(const std::vector<std::vector<char>> & lhs, const std::vector<std::vector<char>> & rhs) {
    for (size_t i = 0 ; i != lhs.size() ; ++i) {
        if (lhs[i].back() != rhs[i].front()) {
            return false;
        }
    }

    return true;
}


bool AltereredTile::adjacent_left_to_right(const AltereredTile & other_tile) const {
    return connect(representation.image, other_tile.representation.image);
}

bool AltereredTile::adjacent_top_to_down(const AltereredTile & other_tile) const {
    return representation.image.back() == other_tile.representation.image.front();
}


std::vector<Path> Tile::adjacent(const Tile & other_tile) const {
    std::vector<Path> paths;

    AltereredTile me { *this };

    std::set<Raw> my_raws;

    for (size_t i = 0 ; i != 4 ; ++i, me.rotate_right()) {
        if (my_raws.contains(me.representation.image)) {
            continue;
        }

        my_raws.insert(me.representation.image);

        std::set<Raw> other_raws;
        AltereredTile other { other_tile };

        for (TileStateExplorer trans ; trans ; trans(other)) {
            if (!other_raws.contains(other.representation.image)) {
                other_raws.insert(other.representation.image);

                if (connect(me.representation.image, other.representation.image)) {
                    paths.push_back(Path { me.transformations.is_rotate_right, other.transformations });
                }
            }
        }
    }

    return paths;
}


std::vector<Tile> to_tiles(const std::vector<std::string> & lines) {
    std::vector<Tile> retval;

    for (const auto & line : lines) {
        if (line == "") continue;

        if (line.rfind("Tile ", 0) == 0) {
            std::string id { line.begin() + 5, line.end() - 1 };
            retval.emplace_back(std::stoi(id));
        } else {
            retval.back().image.emplace_back();

            for (const auto c : line) {
                retval.back().image.back().emplace_back(c);
            }
        }
    }

    return retval;
}


struct TempGrid {
    struct TTile {
        const Tile * tile;
        bool used = false;
    };

    std::vector<std::vector<std::optional<AltereredTile>>> m_grid;
    std::vector<TTile> tiles;
    const AllPaths & all_paths;
    size_t m_size;

    explicit TempGrid(const std::vector<Tile> & p_tiles, const AllPaths & all_paths) : all_paths(all_paths) {
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

    bool try_place_at(size_t x, size_t y) {
        if (y == m_size) return true;

        const size_t next_x = x != m_size - 1 ? x + 1 : 0;
        const size_t next_y = x != m_size - 1 ?     y : y + 1;

        for (auto & [tile, used] : tiles) {
            if (used) continue;

            used = true;

            //const size_t number_of_paths = all_paths.find(tile.id)->second.size();
            //if (min_required_path(x, y) > number_of_paths) continue;

            //if (x != 0 && m_grid[y][x - 1] && no_hope(tile.id, m_grid[y][x - 1]->representation.id)) continue;
            //if (y != 0 && m_grid[y - 1][x] && no_hope(tile.id, m_grid[y - 1][x]->representation.id)) continue;

            m_grid[y][x] = AltereredTile { *tile };

            for (TileStateExplorer trans ; trans ; trans(*m_grid[y][x])) {
                if (x != 0 && !m_grid[y][x - 1]->adjacent_left_to_right(*m_grid[y][x])) {
                    continue;
                }

                if (y != 0 && !m_grid[y - 1][x]->adjacent_top_to_down(*m_grid[y][x])) {
                    continue;
                }

                if (try_place_at(next_x, next_y)) {
                    return true;
                }
            }

            used = false;
        }

        return false;
    }
};

struct Grid {
    std::vector<std::vector<int>> m_grid;

    explicit Grid(const TempGrid & temp_grid) {
        for (const auto & line : temp_grid.m_grid) {
            m_grid.emplace_back();
            for (const auto & value : line) {
                m_grid.back().emplace_back(value->representation.id);
            }
        }
    }

    [[nodiscard]] test::Value corners() const noexcept {
        return test::Value(m_grid.front().front())
            * test::Value(m_grid.front().back())
            * test::Value(m_grid.back().front())
            * test::Value(m_grid.back().back());
    }


    static std::optional<Grid> make(const std::vector<Tile> & tiles, const AllPaths & all_paths) {
        TempGrid temp_grid { tiles, all_paths };

        if (temp_grid.resolve()) {
            return Grid(temp_grid);
        } else {
            return std::nullopt;
        }
    }
};

Output day20(const std::vector<std::string> & lines, const DayExtraInfo &) {
    std::vector<Tile> tiles = to_tiles(lines);

    std::map<int, std::map<int, Paths>> all_paths;

    for (const auto & tile : tiles) {
        for (const auto & tile_ : tiles) {
            if (&tile != &tile_) {
                const auto paths = tile.adjacent(tile_);
                if (!paths.empty()) {
                    all_paths[tile.id][tile_.id] = paths;
                }
            }
        }
    }

    std::optional<Grid> grid = Grid::make(tiles, all_paths);

    if (grid) {
        return Output(grid->corners(), 0);
    } else {
        return Output(0, 0);
    }
}
