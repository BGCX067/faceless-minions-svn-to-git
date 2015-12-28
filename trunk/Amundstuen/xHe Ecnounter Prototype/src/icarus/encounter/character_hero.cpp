#include "icarus/encounter/character_hero.hpp"

namespace icarus
{
namespace encounter
{
character_hero::character_hero(grid* grid_ptr,
                               std::string name,
                               std::string directory)
:
    character_base(grid_ptr, name, directory)
{
    // TODO:: create/read some skills
    move_speed_ = 300.0f;
    health_ = 100;
    current_health_ = health_;
    composite_skill test_skill("Incinerate", 1.0f, 5.0f, 8);
    targeted_skill test_subskill;
    test_subskill.name = "Incinerate initial hit";
    test_subskill.damage = 10;
    test_subskill.type = skill_type::projectile;
    test_subskill.radius = 2;
    test_skill.add_subskill(test_subskill);
    applied_effect test_effect;
    test_effect.name = "Incinerate burning damage";
    test_effect.damage = 5;
    test_effect.duration = 10.0f;
    test_effect.ticks = 5;
    test_effect.type = effect_type::burning;
    test_skill.add_effect(test_effect);
    active_skills_[0] = test_skill;
}

character_hero::~character_hero()
{

}

void character_hero::update(float delta)
{
    for (unsigned i = 0; i < 4; ++i)
        active_skills_[i].update_cooldown(delta);

    for (unsigned i = 0; i < effects_.size(); ++i)
    {
        effects_[i].time_left -= delta;
        if (effects_[i].time_left <= effects_[i].tick_step *
                                     (effects_[i].ticks_remaining - 1))
        {
            effects_[i].ticks_remaining--;
            apply_effect(effects_[i].effect);
        }
        if (effects_[i].time_left <= 0.0f)
            effects_.erase(effects_.begin() + i);
    }
    combat_text_.update(delta);
    hp_bar_.update_bar(delta);
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.6f);
    hp_bar_.set_position(pos);
}

void character_hero::apply_skill(const targeted_skill& skill)
{
    std::cout << name_ << " took " << skill.damage << " damage from " << skill.name << "!" << std::endl;
    if (skill.effect.type != effect_type::undefined)
    {
        active_effect new_effect;
        new_effect.effect = skill.effect;
        new_effect.ticks_remaining = skill.effect.ticks;
        new_effect.time_left = skill.effect.duration;
        new_effect.tick_step = skill.effect.duration / skill.effect.ticks;
        effects_.push_back(new_effect);

        apply_damage(skill.damage);
    }
}

void character_hero::apply_effect(const applied_effect& effect)
{
    apply_damage(effect.damage);
    std::cout << name_ << " took " << effect.damage << " damage from " << effect.name << "!" << std::endl;
}
}   // namespace encounter
}   // namespace icarus
