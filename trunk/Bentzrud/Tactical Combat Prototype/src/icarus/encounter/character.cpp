#include <iostream>
#include <cmath>

#include "icarus/encounter/character.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/encounter/grid.hpp"

namespace icarus
{
namespace encounter
{
void character::set_position(const math::vector2f& new_position)
{
    current_position_ = new_position;
    math::vector2f adjusted_position = current_position_ - anchor_offsets_;
    character_sprite_->setPosition(adjusted_position.get_sf_vector());
    debug_bounds_->setPosition(adjusted_position.get_sf_vector());
}

character::character() :
    character_sprite_(NULL),
    debug_bounds_(NULL),
    debug_enabled_(false),
    anchor_offsets_(0.0f, 0.0f),
    current_position_(0.0f, 0.0f),
    target_position_(0.0f, 0.0f),
    prev_position_(0.0f, 0.0f),
    vector_position_(0, 0),
    move_speed_(100.0f),
    movement_step_(0.0f)
{
}

character::~character()
{
    if (character_sprite_)
        delete character_sprite_;
}

bool character::load_sprite(const std::string& texture_path,
                            const sf::IntRect& texture_coordinates,
                            const math::vector2f char_anchor)
{
    resource_handler::get_instance().load_texture(texture_path,
                                                  "encounter/character/test");
    character_sprite_ = new sf::Sprite(*(resource_handler::get_instance()
                                       .get_texture("encounter/character/test"))
                                       , texture_coordinates);
    anchor_offsets_ = char_anchor;
    // TODO:: use origin instead of anchor_offsets

    debug_bounds_ = new sf::RectangleShape();
    debug_bounds_->setSize(sf::Vector2f(character_sprite_->getLocalBounds().width,
                           character_sprite_->getLocalBounds().height));
    debug_bounds_->setFillColor(sf::Color(0, 0, 0, 0));
    debug_bounds_->setOutlineColor(sf::Color(255, 0, 0, 100));
    debug_bounds_->setOutlineThickness(1.0f);
    return true;
}

void character::draw(sf::RenderTarget& target,
                               sf::RenderStates states) const
{
    target.draw(*character_sprite_, states);
    if (debug_enabled_)
        target.draw(*debug_bounds_, states);
}

void character::tp_to_tile(const math::vector2u& target_tile,
                           const grid* const grid_ptr)
{
    if (vector_position_ == target_tile)
        return;
    grid_tile* target = grid_ptr->get_tile_at(target_tile);
    target->set_occupant(this);
    set_position(target->get_char_anchor());
    vector_position_ = target_tile;
}

void character::update_movement(const grid* const grid_ptr)
{
    if (path_to_target_.size() == 0)
        return;
    grid_tile* target_tile = grid_ptr->get_tile_at(path_to_target_.back()->
                                                   vec_pos);
    if (target_position_ != target_tile->get_char_anchor())
    {
        target_position_ = target_tile->get_char_anchor();
        prev_position_ = current_position_;
    }
    if (current_position_ != target_position_)
    {
        set_position(math::v2lerp(prev_position_, target_position_,
                                  movement_step_));
        movement_step_ += (math::clamp(move_speed_, 0.0f, 1000.0f) * 0.001f);
        if (movement_step_ >= 0.5f)
        {
            if (target_tile->get_vector_pos() != vector_position_)
            {
                target_tile->set_occupant(this);
                grid_ptr->get_tile_at(vector_position_)->set_occupant(NULL);
                vector_position_ = target_tile->get_vector_pos();
            }
        }
    }
    else
    {
        // TODO:: MEMORY LEAKS!
        path_to_target_.pop_back();
        movement_step_ = 0.0f;
    }
}

void character::set_target_path(std::vector<path_node*> new_path)
{
    for (unsigned i = 0; i < path_to_target_.size(); ++i)
        delete path_to_target_[i];
    path_to_target_.clear();

    path_to_target_ = new_path;
}
} // namespace encounter
} // namespace icarus
