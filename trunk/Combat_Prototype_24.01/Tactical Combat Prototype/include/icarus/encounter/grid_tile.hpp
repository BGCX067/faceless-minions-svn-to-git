#ifndef ICARUS_ENCOUNTER_GRID_TILE_HPP_
#define ICARUS_ENCOUNTER_GRID_TILE_HPP_

#include <SFML/Graphics.hpp>

#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace encounter
{

class character;

namespace tile_state
{
enum state
{
    unoccupied,
    occupied,
    blocked
};
} // namespace state

class grid_tile
{
private:
    const sf::Color COLOR_BASE_;          // low-alpha black
    const sf::Color COLOR_HOVER_;         // green
    const sf::Color COLOR_SELECTED_;      // yellow
    const sf::Color COLOR_OCCUPIED_;      // blue
    const sf::Color COLOR_BLOCKED_;       // red

    character* occupant_;
    tile_state::state current_state_;

    math::vector2u vector_position_;
    math::vector2f world_position_;
    math::vector2f character_anchor_;
    sf::RectangleShape *shape_;
public:
    grid_tile(const math::vector2u& vec_pos, const math::vector2f& world_pos);
    ~grid_tile();
    void generate_shape(float width, float height, float spacing);
    void set_hover_state(bool hover);
    sf::RectangleShape* get_shape() const { return shape_; }
    void set_occupant(character* occupant);
    const character* const get_occupant() const { return occupant_; }
    tile_state::state get_state() const { return current_state_; }
    const math::vector2u& get_vector_pos() const { return vector_position_; }
    const math::vector2f& get_world_pos() const { return world_position_; }
    const math::vector2f& get_char_anchor() const { return character_anchor_; }
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_GRID_TILE_HPP_
