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

        utils::yth_node* shadow_node = resource_handler::get()->get_root_node("effects")->child("shadows")->child(data_node->child("shadow")->value());
        sheet_path = sprite_node->child(shadow_node->child("sprite_sheet")->value())->child("path")->value();
        shadow_.load_sprite(sprite_node->child(shadow_node->child("sprite_sheet")->value())->child(shadow_node->child("sprite_name")->value()), sheet_path);

        if (state_manager::get()->get_hd_status())
        {
            character_sprite_->setScale(0.66666f, 0.66666f);
            shadow_.set_scale(0.66666f, 0.66666f);
        }
        speed_ = utils::to_hd(speed_);

        projectile_offset_ = math::vector2f(0.0f, utils::to_hd(-40.0f));
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
    math::vector2f offset = math::vector2f(speed_ * h_ratio_ * delta,
                                           speed_ * v_ratio_ * delta);
    character_sprite_->setPosition(math::vector2f(character_sprite_->getPosition()) + offset);
    if (!is_railgun_)
        shadow_.set_position(shadow_.get_position() + offset);
}
math::vector2f projectile::get_position() const
{
    if (!is_railgun_)
        return shadow_.get_position();
    return character_sprite_->getPosition();
}

math::vector2f projectile::get_projectile_pos() const
{
    return character_sprite_->getPosition();
}

void projectile::set_initial_pos(const math::vector2f& pos)
{
    character_sprite_->setPosition(pos + projectile_offset_);
    if (!is_railgun_)
        shadow_.set_position(pos);
}
void projectile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!is_railgun_)
        target.draw(shadow_, states);
    target.draw(*character_sprite_, states);
}
}   // namespace encounter
} // namespace icarus
