#include "icarus/encounter/animate.hpp"
#include "icarus/resource_handler.hpp"
#include <iostream>

namespace icarus
{
namespace encounter
{
animate::animate() : sprite_width_(100)
                     ,sprite_height_(100)
                     ,sheet_width_(400)
                     ,sheet_height_(200)
                     ,sheet_columns_(0)
                     ,sheet_row_(0)
                     ,target_framerate_(0.0)
                     ,character_sprite_(NULL)

{
    //ctor
    sprite_directory_ = "images/characters/boxSheet.png";
    load_sprite();
}

animate::animate(std::string sprite_dir) : sprite_width_(100)
                    ,sprite_height_(100)
                     ,sheet_width_(400)
                     ,sheet_height_(200)
                     ,sheet_columns_(0)
                     ,sheet_row_(0)
                     ,target_framerate_(0.0)
                     ,character_sprite_(NULL)
{
    std::cout << sprite_dir << std::endl;
    sprite_directory_ = sprite_dir;
    load_sprite();
}

animate::~animate()
{
    //dtor
}

void animate::update_anim()
{
    target_framerate_ = (target_framerate_ + 0.000001);

    if(state_manager::get_instance().get_delta() <= target_framerate_)
    {
        character_sprite_->setTextureRect
        (sf::IntRect(sheet_columns_, sheet_row_, sprite_width_, sprite_height_));
        sheet_columns_ += sprite_width_;
        if(sheet_columns_ >= sheet_width_)
        {
            sheet_columns_ = 0;
        }
        if(sheet_row_ >= sheet_height_)
        {
            sheet_row_ = 0;
        }
        target_framerate_ = 0.0f;
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

void animate::load_sprite()
{
    sf::IntRect rect(0, 0, sprite_width_, sprite_height_);
    sf::Texture* tex = NULL;
    if ((tex = resource_handler::get_instance().get_texture(sprite_directory_)))
        character_sprite_ = new sf::Sprite(*tex, rect);
    character_sprite_->setOrigin((sprite_height_*0.5), (sprite_width_*0.5));
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

void animate::set_animation_state(anim_specs::anim_state new_state)
{
    switch(new_state)
    {
    default:
    case anim_specs::idle:  sheet_row_ = 0;break;
    case anim_specs::walk:  sheet_row_ = 100;break;
    }
    animation_state_ = new_state;
}

}   // namespace encounter
}   // namespace icarus

