#ifndef ICARUS_ENCOUNTER_CHARACTER_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_HPP_

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "icarus/math/vector2.hpp"
#include "icarus/encounter/grid.hpp"

namespace icarus
{
namespace encounter
{

class character : public sf::Drawable
{
private:
    sf::Sprite* character_sprite_;
    sf::RectangleShape* debug_bounds_;
    bool debug_enabled_; // temp until state manager
    math::vector2f anchor_offsets_;
    math::vector2f current_position_;
    math::vector2f target_position_;
    math::vector2f prev_position_;
    math::vector2u vector_position_;
    float move_speed_;
    float movement_step_;
    std::vector<path_node*> path_to_target_;

    void set_position(const math::vector2f& new_position);
public:
    character();
    ~character();

    bool load_sprite(const std::string& texture_path,
                     const sf::IntRect& texture_coordinates,
                     const math::vector2f char_anchor);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void tp_to_tile(const math::vector2u& target_tile,
                    const grid* const grid_ptr);
    void update_movement(const grid* const grid_ptr);
    void set_target_path(std::vector<path_node*> new_path); // NOTE:: check const correctness

    unsigned get_z_depth() const { return vector_position_.y; }
    sf::FloatRect get_bounds(bool use_global = true) const
    {
        if (use_global)
            return character_sprite_->getGlobalBounds();
        return character_sprite_->getLocalBounds();
    }
    math::vector2u get_vector_pos() const { return vector_position_; }
    math::vector2f get_position() const
    {
        return current_position_;
    }
    void set_debug(bool debug_state) { debug_enabled_ = debug_state; } // temp
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_HPP_
