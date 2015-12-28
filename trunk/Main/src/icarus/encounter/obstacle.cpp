#include "icarus/encounter/obstacle.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
obstacle::obstacle(grid* grid_ptr, const std::string& name)
:
    character_base(grid_ptr, name)
{
    utils::yth_node* obstacle_node = resource_handler::get()->get_root_node("obstacles")->child(name);
    health_ = atoi(obstacle_node->child("health")->value().c_str());
    current_health_ = health_;
    is_ai_ = true;
    is_obstacle_ = true;

    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets")->child(obstacle_node->child("sprite_sheet")->value());
    load_sprite(sprite_node->child(name), sprite_node->child("path")->value());
    set_animation_state("default");
}
obstacle::~obstacle()
{

}

bool obstacle::apply_damage(int damage, uint8_t flags, character_base* source)
{
    if (is_dead_)
        return false;

    current_health_ -= 1;

    if (current_health_ < ceil(health_ / 2.0f) &&
        current_health_ > 0 &&
        animation_state_ != "semi_broken")
        {
            set_animation_state("semi_broken");
        }

    print_damage(damage, damage_type::normal);

    if (current_health_ <= 0)
    {
        set_animation_state("destroyed");
        death();
    }
    current_health_ = current_health_ < 0 ? 0 : current_health_;
    update_hp_bar(static_cast<float>(current_health_) / health_);
    return current_health_;
}

void obstacle::update(double delta)
{
    if (is_dead_)
    {
        if (flash_count_ < 5)
        {
            if (flash_timer_.update(delta))
            {
                flash_ = !flash_;
                flash_timer_.reset();
                if (!flash_)
                    flash_count_++;
            }
        }
        else
        {
            render_ = false;
        }
        return;
    }
    combat_text_.update(delta);
}
}   // namespace encounter
}   // namespace icarus

