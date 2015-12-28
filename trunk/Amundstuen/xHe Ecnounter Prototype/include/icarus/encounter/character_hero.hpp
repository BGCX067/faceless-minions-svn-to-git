#ifndef ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_

#include "character_base.hpp"
#include "composite_skill.hpp"

namespace icarus
{
namespace encounter
{
class character_hero : public character_base
{
private:
    // stats
    int strength_, intelligence_, agility_;

    composite_skill active_skills_[4];

protected:
public:
    character_hero(grid* grid_ptr, std::string name, std::string directory);
    character_hero(grid* grid_ptr,
                   unsigned sprite_height,
                   unsigned sprite_width,
                   unsigned sheet_height,
                   unsigned sheet_width,
                   std::string name);
    virtual ~character_hero();

    virtual void update(float delta);
    virtual void apply_skill(const targeted_skill& skill);
    virtual void apply_effect(const applied_effect& effect);

    composite_skill& get_skill(int index) { return active_skills_[index]; }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_
