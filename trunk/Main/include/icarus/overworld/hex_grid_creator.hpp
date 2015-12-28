#ifndef HEX_GRID_CREATOR_H
#define HEX_GRID_CREATOR_H

#include "hex_sprite.hpp"
#include "statistics.hpp"
#include "icarus/utils/serializable.hpp"
#include "nation_info.hpp"
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
    typedef std::vector <hex_sprite*>   tile_group_;

    sf::Texture                         texture_base_;
    sf::Texture                         texture_border_faction_;
    sf::Texture                         texture_border_natural_;
    sf::Texture                         texture_overlay_;
    statistics*                         stats;
    hex_sprite*                         spawn_point_;

    std::vector<battle_info::battlezone_info> temp_battlezones_;

public:
    std::vector<sf::Vector2i>           sprite_base_;
    std::vector<sf::Vector2i>           sprite_border_;
    std::vector<sf::Vector2i>           sprite_overlay_;
    hex_grid_creator();

    virtual ~hex_grid_creator();

    void add_surrounding_tiles_to(std::vector<hex_hold_>& hex_grid_,
                          unsigned source_row, unsigned source_col);
    //
    void create_hex_grid(std::vector<hex_hold_>& hex_grid_);

    void asign_nearby_tiles(std::vector<hex_hold_>& hex_grid_);

    hex_sprite* return_sprite(std::vector<hex_hold_>& hex_grid_,
                    const unsigned row, const unsigned col) {return &hex_grid_[row][col];}
    //
    const hex_sprite* return_const_sprite(const std::vector<hex_hold_>& hex_grid_,
                                const unsigned row, const unsigned col) const   {return &hex_grid_[row][col];}
    //
    const sf::FloatRect pass_bounds(std::vector<hex_hold_>& hex_grid_,
                                const unsigned row, const unsigned col) const;
    //
    const overworld::state pass_state(const std::vector<hex_hold_>& hex_grid_,
                                const unsigned row, const unsigned col) const   {return(hex_grid_[row][col].get_occupation());}
    //
    bool is_inside_bounds(const unsigned row, const unsigned col);
    //
    void generate_starter_fractions(std::vector<hex_hold_>& hex_grid_,
                                    std::vector<tile_group_>& nation_tiles_,
                                    const unsigned num_frac,
                                    const double percentage_uncaptured = 0.30);
    //
    void generate_ground_ranges(std::vector<hex_hold_>& hex_hold_,
                                  std::vector<tile_group_>& nation_tiles_,
                                  const unsigned tile_id,
                                  unsigned number_of_spawn_areas,
                                  const double percentage_beeing_this = 0.05,
                                  const unsigned max_bordering = 2,
                                  const bool require_border = false);
    //
    void generate_captial_cities(tile_group_& group_tiles,
                                    const overworld::type_frac frac_id);
    //
    void change_nearby_tile_to_same(std::vector<hex_hold_>& hex_grid_,
                                    tile_group_& group,
                                    const unsigned type_tile,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border = 6);
    //
    hex_sprite* get_spawn() {return spawn_point_;}
    void variate_background_tiles(std::vector<hex_hold_>& hex_grid_);
    //
    void set_borders(std::vector<hex_hold_>& hex_grid_);
    //
    void setup_overlay(std::vector<hex_hold_>& hex_grid_,
                       std::vector<unsigned> num_each);
    //
    void set_surrounding_to(
                overworld::type_ id,
                hex_sprite* target_tile,
                unsigned type,
                unsigned this_size,
                unsigned max_size);
    //
    void uppdate_surrounding_border(hex_sprite* target_tile, unsigned this_size, unsigned max_size);
    //
    std::vector<hex_sprite*> return_list(std::vector<hex_hold_>& hex_grid_);

    void setup_presense(std::vector<hex_hold_>& hex_grid, std::vector<tile_group_>& nations);

    bool add_structure(tile_group_& nations, overworld::type_overlay id);
    bool remove_structure(hex_sprite* tile);
    //
    std::vector<sf::Vector2i>& get_overlay_sprire() {return sprite_overlay_;}
    void setup_importance(std::vector<hex_hold_>& a);
    void set_single_border(hex_sprite* tile, bool changeback = false);

    void load_grid( const std::vector<utils::grid_tile>& tiles,
                    std::vector<hex_hold_>& grid,
                    std::vector<nation_info>& nations,
                    std::vector<tile_group_>& nation_list);

    std::vector<battle_info::battlezone_info>& get_battlezones_from_save(){return temp_battlezones_;}
};

} // namespace overworld
} // namespace icarus

#endif // HEX_GRID_CREATOR_H
