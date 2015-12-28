#include "icarus/encounter/animate.hpp"
#include "icarus/resource_handler.hpp"
#include <iostream>

namespace icarus
{
namespace encounter
{
animate::animate()
:
    time_elapsed_(0.0),
    target_framerate_(12),
    character_sprite_(NULL)
{

}
animate::~animate()
{
    //dtor
    // TODO: fix this!
//    if (character_sprite_)
//        delete character_sprite_;
}

void animate::update_anim()
{
    time_elapsed_ += state_manager::get()->get_delta();
    if(time_elapsed_ >= 1.0/static_cast<double>(target_framerate_))
    {
        // increment frame counter, reset if necessary
        current_frame_++;
        if (current_frame_ >= active_animation_.frame_count_)
            current_frame_ = 0;
        // changed to use the new active_animation_ struct
        int x = active_animation_.start_x_ + (current_frame_ * active_animation_.frame_width_);
        int y = active_animation_.start_y_;

        character_sprite_->setTextureRect(sf::IntRect(x, y,
                                                      active_animation_.frame_width_,
                                                      active_animation_.frame_height_));
        time_elapsed_ = 0.0;
    }
}

void animate::update_pos(const math::vector2f& sprite_pos_updated,
                         const math::vector2i& grid_pos_updated)
{
    character_sprite_->setPosition(sprite_pos_updated.get_sf_vector());
    char_grid_pos_ = grid_pos_updated;
}

void animate::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*character_sprite_, states);
}

void animate::load_sprite(utils::yth_node* sprite_node,
                          const std::string& sheet_path)
{
    // loop through animations and register all the valid ones
    for (unsigned i = 0; i < sprite_node->child_count(); ++i)
    {
        // create temporary struct to hold animation information, then
        // load animation info from the yth_node
        animation temp_anim;
        temp_anim.start_x_ =        atoi(sprite_node->child(i)->child("start_x")->value().c_str());
        temp_anim.start_y_ =        atoi(sprite_node->child(i)->child("start_y")->value().c_str());
        temp_anim.frame_width_ =    atoi(sprite_node->child(i)->child("frame_width")->value().c_str());
        temp_anim.frame_height_ =   atoi(sprite_node->child(i)->child("frame_height")->value().c_str());
        temp_anim.origin_x_ =       atoi(sprite_node->child(i)->child("origin_x")->value().c_str());
        temp_anim.origin_y_ =       atoi(sprite_node->child(i)->child("origin_y")->value().c_str());
        temp_anim.frame_count_ =    atoi(sprite_node->child(i)->child("frames")->value().c_str());

        std::string key = sprite_node->child(i)->key();

        // check if this animation is already in the map
        // continue without storing information if this is the case
        if (anim_map_.count(key) > 0)
            continue;

        // pair the key and animation, then store in the map
        std::pair<std::string, animation> new_pair(key, temp_anim);
        anim_map_.insert(new_pair);
    }

    // load sprite sheet from the resource handler
    sf::Texture* tex = NULL;
    if ((tex = resource_handler::get()->get_texture(sheet_path)))
        character_sprite_ = new sf::Sprite(*tex);

    set_animation_state(anim_map_.begin()->first);
// old code
//    if ((tex = resource_handler::get()->get_texture(sprite_directory_)))
//        character_sprite_ = new sf::Sprite(*tex, rect);
}

void animate::orient_sprite(bool positive_axis)
{
    if(positive_axis == false)
        character_sprite_->setScale(-1.0, 1.0);
    else
        character_sprite_->setScale(1.0, 1.0);
}

math::vector2f animate::get_world_pos()
{
    return math::vector2f(character_sprite_->getPosition());
}

void animate::set_world_pos(const math::vector2f& new_pos)
{
    character_sprite_->setPosition(new_pos.get_sf_vector());
}

void animate::set_animation_state(const std::string& new_state)
{
    // set the active animation and state
    if (anim_map_.count(new_state) <= 0)
        return;
    // checking to see if this animation is registered for this actor
    active_animation_ = anim_map_.at(new_state);
    animation_state_ = new_state;
    current_frame_ = 0;
    int x = active_animation_.start_x_;
    int y = active_animation_.start_y_;

    character_sprite_->setTextureRect(sf::IntRect(x, y,
                                                  active_animation_.frame_width_,
                                                  active_animation_.frame_height_));if (active_animation_.origin_x_ != 0 &&
    active_animation_.origin_y_ != 0)
    character_sprite_->setOrigin(active_animation_.origin_x_,
                                 active_animation_.origin_y_);
}

}   // namespace encounter
}   // namespace icarus

