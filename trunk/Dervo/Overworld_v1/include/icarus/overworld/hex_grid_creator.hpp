#ifndef HEX_GRID_CREATOR_H
#define HEX_GRID_CREATOR_H

#include "hex_sprite.hpp"
#include "statistics.hpp"
#include <string>
#include <ctime>
#include <iostream>

namespace icarus
{
namespace overworld
{
class hex_grid_creator
{
private:
    //hex_grid_creator* this_grid_creator;
    typedef std::vector<hex_sprite>     hex_hold_;
    std::vector<hex_hold_>              hex_grid_;
    std::vector<sf::Vector2i>           sprite_holder_;
    sf::Texture                         grid_texture_file_;
    typedef std::vector <hex_sprite*>   nation_group_;
    std::vector<nation_group_>          nation_tiles_;
    std::vector<nation_group_>          nature_tiles_;
    statistics                          stats;
    hex_sprite*                         spawn_point_;

public:
    hex_grid_creator();
    virtual ~hex_grid_creator();
    void change_variables(const unsigned *x, const unsigned *y);
    void create_hex_grid();
    void asign_nearby_tiles();
    hex_sprite* return_sprite(const unsigned row,
                                    const unsigned col);
    const hex_sprite* return_const_sprite(const unsigned row,
                                    const unsigned col) const;
    const overworld::state pass_state(const unsigned row, const unsigned col) const;
    const sf::FloatRect pass_bounds(const unsigned row, const unsigned col);
    bool is_inside_bounds(const unsigned row, const unsigned col);
    void generate_starter_fractions(const unsigned num_frac,
                                    const double percentage_uncaptured = 0.30);
    void set_tile_to(const hex_sprite* active_tile, const unsigned id);
    void change_nearby_tile_to_same(nation_group_& group,
                                    const unsigned type_nation,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border = 6);
    void generate_fraction_home(const unsigned type_nation);
    void generate_nature_tiles(const unsigned tile_id,
                               const unsigned number_of_spawn_areas,
                               const double percentage_beeing_this = 0.05,
                               const unsigned max_bordering = 2,
                               const bool require_border = false);
    bool is_path_aviable(const nation_group_ landmarks);
    void change_surrounding_to(const unsigned type, hex_sprite* hex);
    void variate_background_tiles();
    void generate_captial_cities(const unsigned tile_id,
                                 const unsigned group);
    hex_sprite* get_spawn();
    //hex_sprite* hit_tile(math::vector2f coordinates);

protected:

};

} // namespace overworld
} // namespace icarus

#endif // HEX_GRID_CREATOR_H
