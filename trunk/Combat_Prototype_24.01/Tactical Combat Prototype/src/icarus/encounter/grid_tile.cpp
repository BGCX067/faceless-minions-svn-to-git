#include "icarus/encounter/grid_tile.hpp"

namespace icarus
{
namespace encounter
{
grid_tile::grid_tile(const math::vector2u& vec_pos,
                     const math::vector2f& world_pos)
:
    COLOR_BASE_(0, 0, 0, 0),
    COLOR_HOVER_(0, 255, 0, 50),
    COLOR_SELECTED_(255, 255, 0, 50),
    COLOR_OCCUPIED_(0, 0, 255, 50),
    COLOR_BLOCKED_(255, 0, 0, 50),
    occupant_(NULL),
    current_state_(tile_state::unoccupied),
    vector_position_(vec_pos),
    world_position_(world_pos),
    character_anchor_(0, 0),
    shape_(NULL)
{
}

grid_tile::~grid_tile()
{
}

void grid_tile::generate_shape(float width, float height, float spacing)
{
    shape_ = new sf::RectangleShape;

    shape_->setOutlineThickness(spacing);
    shape_->setOutlineColor(sf::Color(0, 0, 0, 20));
    shape_->setFillColor(COLOR_BASE_);
    shape_->setPosition(world_position_.get_sf_vector());
    shape_->setSize(sf::Vector2f(width, height));
    character_anchor_ = world_position_ + math::vector2f(width * 0.5f,
                                                       height * 0.5f);
}

void grid_tile::set_hover_state(bool hover)
{
    if (!hover)
    {
        shape_->setFillColor(COLOR_BASE_);
        return;
    }
    else
    {
        switch (current_state_)
        {
        case tile_state::unoccupied:
            shape_->setFillColor(COLOR_HOVER_);
            break;
        case tile_state::occupied:
            shape_->setFillColor(COLOR_OCCUPIED_);
            break;
        case tile_state::blocked:
            shape_->setFillColor(COLOR_BLOCKED_);
            break;
        }
    }
}

void grid_tile::set_occupant(character* occupant)
{
    occupant_ = occupant;
    current_state_ = (occupant ? tile_state::occupied : tile_state::unoccupied);
}
} // namespace encounter
} // namespace icarus
