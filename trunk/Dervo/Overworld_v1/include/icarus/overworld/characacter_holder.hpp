#ifndef CHARACACTER_HOLDER_H
#define CHARACACTER_HOLDER_H


#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "hex_sprite.hpp"
#include "statistics.hpp"

#include <iostream>

namespace icarus
{
namespace overworld
{
enum type_vector2f { CURRENT, TARGET, PREV};

class characacter_holder : sf::Drawable
{
private:
    math::vector2f current_position_;
    math::vector2f target_position_;
    math::vector2f prev_position_;
    math::vector2i vector_position_;

    math::vector2f adjustment_position_;


    float move_speed_;
    float movement_step_;

    sf::Sprite character_sprite_;
    sf::Texture character_texture_;
    statistics stats;

    hex_sprite* placed_at;

protected:
public:
    characacter_holder();
    virtual ~characacter_holder();

    void setup_character(hex_sprite* spawn_point);

    math::vector2f  get_vec2f_position(type_vector2f type);
    void            set_vec2f_position(type_vector2f type, math::vector2f new_value);
    math::vector2i  get_vec2i_position();
    void            set_vec2i_position(math::vector2i new_value);

    void update_character(); // skal oppdateres hver frame
    void move_to(hex_sprite* target_point); // set target_position_

    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
    const sf::Sprite* return_sprite() const;
    void set_surrounding_tiles_to(overworld::state type);
};
} // namespace overworld
} // namespace icarus

#endif // CHARACACTER_HOLDER_H
