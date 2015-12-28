#include "icarus/encounter/projectile.hpp"

namespace icarus
{
namespace encounter
{
projectile::projectile(float speed)
:
    animate(),
    speed_(speed),
    v_ratio_(0.0f),
    h_ratio_(0.0f),
    projectile_offset_(0.0f, 0.0f),
    caster_(NULL)
{
}

projectile::~projectile()
{
}

void projectile::set_skill(const targeted_skill& skill)
{
    subskill_ = skill;
}

void projectile::set_speed_ratios(float h_ratio, float v_ratio)
{
    h_ratio_ = h_ratio;
    v_ratio_ = v_ratio;
}

void projectile::update(float delta)
{
    math::vector2f new_pos = (math::vector2f(character_sprite_->getPosition())
                              + projectile_offset_)
                              + math::vector2f(speed_ * h_ratio_ * delta,
                                               speed_ * v_ratio_ * delta);
    character_sprite_->setPosition(new_pos.get_sf_vector());
}
}   // namespace encounter
} // namespace icarus
