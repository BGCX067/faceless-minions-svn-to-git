#include "icarus/encounter/visual_effect.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
visual_effect::visual_effect(math::vector2f position,
                             utils::yth_node* data,
                             bool loop,
                             float timer,
                             bool unscaled)
:
    loop_(loop),
    timer_(timer),
    time_remaining_(timer),
    active_(true),
    loop_counter_(0),
    loop_counted_(false)
{
    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets");
    std::string sheet_path = sprite_node->child(data->child("sprite_sheet")->value())->child("path")->value();
    utils::yth_node* animation_node = sprite_node->child(data->child("sprite_sheet")->value())->child(data->child("sprite_name")->value());
    load_sprite(animation_node, sheet_path);

    if (!state_manager::get()->get_hd_status() && !unscaled)
    {
        character_sprite_->setScale(0.66666f, 0.66666f);
    }
    character_sprite_->setPosition(position.get_sf_vector());
}
visual_effect::~visual_effect()
{
}

void visual_effect::update(double delta)
{
    if (current_frame_ == 0 && !loop_counted_)
    {
        loop_counted_ = true;
        loop_counter_++;
    }
    if (current_frame_ != 0)
        loop_counted_ = false;

    if (loop_ || (time_remaining_ > 0.0f && timer_ != 0.0f) ||
        (!loop_ && timer_ == 0.0f && loop_counter_ < 2))
    {
        update_anim();
        time_remaining_ -= delta;
    }
    else
    {
        active_ = false;
    }
}

void visual_effect::reset()
{
    active_ = true;
    current_frame_ = 0;
    loop_counted_ = false;
    loop_counter_ = 0;
    time_remaining_ = timer_;
}
void visual_effect::stop()
{
    active_ = false;
    current_frame_ = 0;
    loop_counted_ = false;
    loop_counter_ = 0;
    time_remaining_ = timer_;
}
}   // namespace encounter
} // namespace icarus
