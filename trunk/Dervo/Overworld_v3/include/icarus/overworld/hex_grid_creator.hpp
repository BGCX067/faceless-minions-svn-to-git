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
    typedef std::vector<hex_sprite>     hex_hold_;
    sf::Texture                         texture_nature_;
    sf::Texture                         texture_border_;
    sf::Texture                         texture_factions_;
    sf::Texture                         texture_structures_;
    typedef std::vector <hex_sprite*>   nation_group_;
    statistics                          stats;
    hex_sprite*                         spawn_point_;

public:
    std::vector<sf::Vector2i>           sprite_nature_;
    std::vector<sf::Vector2i>           sprite_border_;
    std::vector<sf::Vector2i>           sprite_factions_;
    std::vector<sf::Vector2i>           sprite_structures_;
    hex_grid_creator();

    virtual ~hex_grid_creator();

    void change_variables(std::vector<hex_hold_>& hex_grid_,
                          const unsigned *x,
                          const unsigned *y);
    //
    void create_hex_grid(std::vector<hex_hold_>& hex_grid_);

    void asign_nearby_tiles(std::vector<hex_hold_>& hex_grid_);

    hex_sprite* return_sprite(std::vector<hex_hold_>& hex_grid_,
                              const unsigned row,
                              const unsigned col);
    //
    const hex_sprite* return_const_sprite(
                                    const std::vector<hex_hold_>& hex_grid_,
                                    const unsigned row,
                                    const unsigned col) const;
    //
    const sf::FloatRect pass_bounds(std::vector<hex_hold_>& hex_grid_,
                                    const unsigned row,
                                    const unsigned col) const;
    //
    const overworld::state pass_state(const std::vector<hex_hold_>& hex_grid_,
                                       const unsigned row,
                                       const unsigned col) const;
    //
    bool is_inside_bounds(const unsigned row,
                          const unsigned col);
    //
    void generate_starter_fractions(std::vector<hex_hold_>& hex_grid_,
                                    std::vector<nation_group_>& nation_tiles_,
                                    const unsigned num_frac,
                                    const double percentage_uncaptured = 0.30);
    //
    void generate_nature_tiles(std::vector<hex_hold_>& hex_grid_,
                               std::vector<nation_group_>& nation_tiles_,
                               const unsigned tile_id,
                               const unsigned number_of_spawn_areas,
                               const double percentage_beeing_this = 0.05,
                               const unsigned max_bordering = 2,
                               const bool require_border = false);
    //
    void generate_captial_cities(nation_group_& group_tiles,
                                 const unsigned tile_id,
                                 const unsigned group);
    //
    void set_tile_to(const hex_sprite* active_tile,
                     const unsigned id);
    //
    void change_nearby_tile_to_same(std::vector<hex_hold_>& hex_grid_,
                                    nation_group_& group,
                                    const overworld::nation type_nation,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border = 6);
    //
    void change_nearby_tile_to_same(std::vector<hex_hold_>& hex_grid_,
                                    nation_group_& group,
                                    const unsigned type_nation,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border = 6);
    //
    void generate_fraction_home(const unsigned type_nation);
    //
    bool is_path_aviable(const nation_group_ landmarks);

    void change_surrounding_to(const unsigned type,
                               hex_sprite* hex,
                               sf::Vector2i* tex);
    //
    void variate_background_tiles(std::vector<hex_hold_>& hex_grid_);
    //
    hex_sprite* get_spawn();
    void set_borders(std::vector<hex_hold_>& hex_grid_);
    //hex_sprite* hit_tile(math::vector2f coordinates);
    //nation_group_& return_fraction_tiles(overworld::nation type);

protected:

};

} // namespace overworld
} // namespace icarus

#endif // HEX_GRID_CREATOR_H
