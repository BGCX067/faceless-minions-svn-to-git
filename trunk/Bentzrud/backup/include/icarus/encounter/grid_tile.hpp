#ifndef ICARUS_ENCOUNTER_GRID_TILE_HPP_
#define ICARUS_ENCOUNTER_GRID_TILE_HPP_

#include <SFML/Graphics.hpp>

#include "icarus/math/vector2.hpp"
#include "icarus/encounter/character_base.hpp"

namespace icarus
{
namespace encounter
{

namespace tile_state
{
enum state
{
    unoccupied,
    occupied,
    blocked,
    reserved
};
} // namespace state

class grid_tile : public sf::Drawable
{
private:
    character_base* occupant_;
    tile_state::state current_state_;
    tile_state::state prev_state_;

    math::vector2i vector_position_;
    math::vector2f world_position_;
    sf::Sprite* tile_;
    sf::FloatRect bounds_;
    bool hover_;

    bool debug_enabled_;
    sf::RectangleShape* debug_bounds_;
public:
    grid_tile(const math::vector2i& vec_pos, const math::vector2f& world_pos);
    ~grid_tile();
    void load_sprite();
    void set_hover_state(bool hover);
    sf::Sprite* get_sprite() const { return tile_; }
    const sf::FloatRect& get_bounds() const { return bounds_; }
    void set_occupant(character_base* occupant);
    character_base* const get_occupant() const { return occupant_; }
    tile_state::state get_state() const { return current_state_; }
    const math::vector2i& get_vector_pos() const { return vector_position_; }
    const math::vector2f& get_world_pos() const { return world_position_; }
    void reserve_tile() { current_state_ = tile_state::reserved; }
    void unreserve();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    int get_z_coord() const { return -(vector_position_.x + vector_position_.y); }
    int get_z_coord(const math::vector2i& vec) const
    { return -(vec.x + vec.y); }
    unsigned get_range_from(const math::vector2i& tile_coord) const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_GRID_TILE_HPP_
