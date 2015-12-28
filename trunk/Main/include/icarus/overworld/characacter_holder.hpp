#ifndef CHARACACTER_HOLDER_H
#define CHARACACTER_HOLDER_H


#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/overworld/nation_creator.hpp"
#include "icarus/overworld/hex_sprite.hpp"
#include "icarus/overworld/statistics.hpp"
#include "icarus/overworld/encounter_calculator.hpp"
#include "icarus/ui/sprite_sheet.hpp"

#include <iostream>

namespace icarus
{
namespace overworld
{
enum type_vector2f { CURRENT, TARGET, PREV};

class characacter_holder : public sf::Drawable
{
private:
    math::vector2f current_position_;
    math::vector2f target_position_;
    math::vector2f prev_position_;
    math::vector2i vector_position_;

    math::vector2f adjustment_position_;


    float move_speed_;
    float movement_step_;

    sf::Sprite player_indicator_;
    ui::sprite_sheet character_sprite_;
    sf::Texture character_texture_;
    statistics* stats;

    hex_sprite* placed_at;
    bool tiger_;

    //encounter
    encounter_calculator encounter_list;

protected:
public:
    characacter_holder();
    virtual ~characacter_holder();

    void setup_character(hex_sprite* spawn_point, utils::yth_node* data, utils::yth_node* type_reference,nation_creator& nations,
                                         unsigned tiles_moved =0);

    math::vector2f  get_vec2f_position(type_vector2f type);
    void            set_vec2f_position(type_vector2f type, math::vector2f new_value);
    math::vector2i  get_vec2i_position();
    void            set_vec2i_position(math::vector2i new_value);

    void update_character(nation_creator& nations, std::vector<nation_info>& n_info); // skal oppdateres hver frame
    void move_to(hex_sprite* target_point); // set target_position_

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    const ui::sprite_sheet* return_sprite() const;
    void set_surrounding_tiles_to(overworld::state type);
    void runn_trough(hex_sprite* target_tile,
                 overworld::state type,
                 unsigned this_size,
                 unsigned max_size);
    encounter_calculator* return_encounter();
    void enable_tiger();
    hex_sprite* get_current_position()  {return placed_at;}
};
} // namespace overworld
} // namespace icarus

#endif // CHARACACTER_HOLDER_H