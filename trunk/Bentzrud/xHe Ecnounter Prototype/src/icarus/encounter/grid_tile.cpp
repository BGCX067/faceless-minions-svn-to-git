#include "icarus/encounter/grid_tile.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
grid_tile::grid_tile(const math::vector2i& vec_pos,
                     const math::vector2f& world_pos)
:
    occupant_(NULL),
    current_state_(tile_state::unoccupied),
    prev_state_(tile_state::unoccupied),
    vector_position_(vec_pos),
    world_position_(world_pos),
    tile_(NULL),
    bounds_(0, 0, 0, 0),
    hover_(false),
    debug_enabled_(false),
    debug_bounds_(NULL)
{
}

grid_tile::~grid_tile()
{
}

void grid_tile::load_sprite()
{
    sf::IntRect rect(0, 0, 50, 40);
    sf::Texture* tex = NULL;
    if ((tex = resource_handler::get()->get_texture("images/interface/small_tiles.png")))
        tile_ = new sf::Sprite(*tex, rect);
    tile_->setOrigin(25, 20);
    tile_->setPosition(world_position_.get_sf_vector());
    sf::FloatRect bounds(world_position_.x - 25, world_position_.y - 20, 50, 40);
    bounds_ = bounds;

    debug_bounds_ = new sf::RectangleShape();
    debug_bounds_->setSize(sf::Vector2f(bounds.width, bounds.height));
    debug_bounds_->setPosition(bounds.left, bounds.top);
    debug_bounds_->setFillColor(utils::argb(0x00000000));
    debug_bounds_->setOutlineColor(utils::argb(0x64FF0000));
    debug_bounds_->setOutlineThickness(1.0f);
}

void grid_tile::set_hover_state(bool hover)
{
    int frame = 0;
    hover_ = hover;
    if (hover)
    {
        switch (current_state_)
        {
        case tile_state::unoccupied:
            frame = 4;
            break;
        case tile_state::occupied:
            frame = 3;
            break;
        case tile_state::blocked:
            frame = 1;
            break;
        case tile_state::reserved:
            frame = 2;
            break;
        case tile_state::out_of_bounds:
            frame = 0;
            break;
        }
    }
    tile_->setTextureRect(sf::IntRect(50 * frame, 0, 50, 40));
}

void grid_tile::set_occupant(character_base* occupant)
{
    occupant_ = occupant;
    if (occupant_)
    {
        prev_state_ = (current_state_ == tile_state::reserved ?
                       prev_state_ : current_state_);
        current_state_ = tile_state::occupied;
    }
    else
    {
        current_state_ = prev_state_;
    }
}

void grid_tile::unreserve()
{
     if (current_state_ == tile_state::occupied &&
         prev_state_ == tile_state::reserved)
        prev_state_ = tile_state::unoccupied;
     else if (current_state_ == tile_state::reserved)
        current_state_ = tile_state::unoccupied;
}

void grid_tile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*tile_, states);
    if (debug_enabled_ && hover_)
        target.draw(*debug_bounds_, states);
}

unsigned grid_tile::get_range_from(const math::vector2i& tile_coord) const
{
    int x = abs(vector_position_.x - tile_coord.x);
    int y = abs(vector_position_.y - tile_coord.y);
    int z = abs(get_z_coord() - get_z_coord(tile_coord));
    return static_cast<unsigned>(math::highest(x, y, z));
}
} // namespace encounter
} // namespace icarus
