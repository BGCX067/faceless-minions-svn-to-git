#ifndef ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_

#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter/composite_skill.hpp"
#include "icarus/encounter/data.hpp"
#include "icarus/encounter/ui/portrait.hpp"

namespace icarus
{
namespace encounter
{
class character_hero : public character_base
{
private:
    // stats
    int strength_, intelligence_, agility_;
    int str_mod_, int_mod_, agi_mod_;
    hero_class::type class_;
    int modified_hp_;
    std::string surname_;

    // attribute modifiers
    const float STR_DAMAGE_MOD;
    const float STR_HEALTH_MOD;
    const float AGI_SPEED_MOD;
    const float AGI_COOLDOWN_MOD;
    const float AGI_DODGE_MOD;
    const float AGI_CRITICAL_MOD;
    const float AGI_RESIST_MOD;
    const float INT_DAMAGE_MOD;
    const float INT_CRITICAL_MOD;
    const float INT_RESIST_MOD;

    composite_skill active_skills_[5];
    ui::portrait* portrait_;
    bool hover_;

    void setup_skills(const std::string& char_class, const unsigned (&levels)[4]);
    void setup_portrait(unsigned index);
    virtual void update_hp_bar(float fraction);

public:
    character_hero(grid* grid_ptr, std::string name);
    character_hero(grid* grid_ptr,
                   unsigned sprite_height,
                   unsigned sprite_width,
                   unsigned sheet_height,
                   unsigned sheet_width,
                   std::string name);
    virtual ~character_hero();

    virtual void update(double delta);
    void setup_from_data(const hero_data& data, unsigned index);
    void get_data(hero_data& data);
    virtual void draw(sf::RenderTarget& target,
                      sf::RenderStates states) const;
    virtual void update_cast_bar(float remaining, float duration);
    sf::Sprite* get_portrait_ptr();
    virtual std::string get_name(unsigned char_limit = 0) const;

    composite_skill& get_skill(int index) { return active_skills_[index]; }
    void set_hover(bool state) { hover_ = state; }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_
