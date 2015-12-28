#include "icarus/encounter/projectile.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
projectile::projectile(float speed, utils::yth_node* data_node,
                       bool railgun, unsigned range)
:
    animate(),
    speed_(speed),
    v_ratio_(0.0f),
    h_ratio_(0.0f),
    projectile_offset_(0.0f, 0.0f),
    caster_(NULL),
    is_railgun_(railgun),
    range_(range)
{
    if (!is_railgun_)
    {
        utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets");
        std::string sheet_path = sprite_node->child(data_node->child("sprite_sheet")->value())->child("path")->value();
        utils::yth_node* animation_node = sprite_node->child(data_node->child("sprite_sheet")->value())->child(data_node->child("sprite_name")->value());
        load_sprite(animation_node, sheet_path);
    }
    else
    {
        character_sprite_ = new sf::Sprite();
    }
}

projectile::~projectile()
{
}

void projectile::set_skill(const targeted_skill& skill)
{
    subskill_ = skill;
}

void projectile::set_speed_ratios(float angle)
{
    angle_ = angle;
    character_sprite_->setRotation(math::rad_to_deg(-angle)+90);
    h_ratio_ = sin(angle);
    v_ratio_ = cos(angle);
}

void projectile::update(float delta)
{
    update_anim();
    math::vector2f new_pos = (math::vector2f(character_sprite_->getPosition())
                              + projectile_offset_)
                              + math::vector2f(speed_ * h_ratio_ * delta,
                                               speed_ * v_ratio_ * delta);
    character_sprite_->setPosition(new_pos.get_sf_vector());
}
}   // namespace encounter
} // namespace icarus
