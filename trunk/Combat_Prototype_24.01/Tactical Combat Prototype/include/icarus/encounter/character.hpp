#ifndef ICARUS_ENCOUNTER_CHARACTER_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_HPP_

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "grid.hpp"

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
    unsigned sprite_width;
    unsigned sprite_height;
    unsigned sheet_width;
    unsigned sheet_height;
    unsigned sheet_column;
    unsigned sheet_row;

    struct path_node
    {
        int f, g, h;
        math::vector2u vec_pos;
        path_node* parent;
        path_node(math::vector2u v_pos)
        {
            f = g = h = 0;
            parent = NULL;
            vec_pos = v_pos;
        }
        void update_f()
        {
            f = g + h;
        }
    };
    std::vector<path_node*> path_to_target_;

    void set_position(const math::vector2f& new_position);

    void add_surrounding_tiles(const math::vector2u& vec_pos,
                               const grid* const grid_ptr,
                               std::vector<path_node*>& node_vec);
    int compute_h(const math::vector2u& start_pos,
                  const math::vector2u& end_pos) const;
public:
    character();
    ~character();

    bool load_sprite(const std::string& texture_path,
                     const sf::IntRect& texture_coordinates,
                     const math::vector2f char_anchor);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void animate();

    void move_to_tile(const math::vector2u& target_tile,
                      const grid* const grid_ptr);
    void tp_to_tile(const math::vector2u& target_tile,
                    const grid* const grid_ptr);
    void update_movement(const grid* const grid_ptr);
    unsigned get_z_depth() const { return vector_position_.y; }
    sf::FloatRect get_bounds(bool use_global = true) const
    {
        if (use_global)
            return character_sprite_->getGlobalBounds();
        return character_sprite_->getLocalBounds();
    }
    math::vector2f get_position() const
    {
        return current_position_;
    }
    void set_debug(bool debug_state) { debug_enabled_ = debug_state; } // temp
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_HPP_
