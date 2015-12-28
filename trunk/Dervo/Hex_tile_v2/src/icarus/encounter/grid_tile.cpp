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
    utils::yth_node* grid_node = resource_handler::get()->get_root_node("ui")->child("grid");
    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets");

    tile_.load_sprite(sprite_node->child(grid_node->child("grid_tile")->value())->child(grid_node->child("grid_tile")->key()),
                        sprite_node->child(grid_node->child("grid_tile")->value())->child("path")->value());

    tile_.set_position(world_position_);
    tile_.set_sprite("default");

    debug_bounds_ = new sf::RectangleShape();
    debug_bounds_->setSize(sf::Vector2f(tile_.get_bounds().width, tile_.get_bounds().height));
    debug_bounds_->setPosition(tile_.get_bounds().left, tile_.get_bounds().top);
    debug_bounds_->setFillColor(utils::argb(0x00000000));
    debug_bounds_->setOutlineColor(utils::argb(0x64FF0000));
    debug_bounds_->setOutlineThickness(1.0f);
}

void grid_tile::set_hover_state(bool hover)
{
    hover_ = hover;
    if (hover)
    {
        switch (current_state_)
        {
        case tile_state::unoccupied:
            tile_.set_sprite("available");
            break;
        case tile_state::occupied:
            tile_.set_sprite("occupied");
            break;
        case tile_state::blocked:
            tile_.set_sprite("blocked");
            break;
        case tile_state::reserved:
            tile_.set_sprite("reserved");
            break;
        case tile_state::out_of_bounds:
            tile_.set_sprite("out_of_bounds");
            break;
        }
    }
    else
    {
        if (current_state_ == tile_state::out_of_bounds)
        {
            tile_.set_sprite("out_of_bounds");
        }
        else
        {
            tile_.set_sprite("default");
        }
    }
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
    target.draw(tile_, states);
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

void grid_tile::set_state(tile_state::state new_state)
{
    current_state_ = new_state;
    if (new_state == tile_state::out_of_bounds)
        tile_.set_sprite("out_of_bounds");
}
} // namespace encounter
} // namespace icarus
