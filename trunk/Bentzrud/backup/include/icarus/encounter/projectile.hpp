#ifndef ICARUS_ENCOUNTER_PROJECTILE_HPP_
#define ICARUS_ENCOUNTER_PROJECTILE_HPP_

#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter.hpp"

namespace icarus
{
namespace encounter
{
class projectile : public animate
{
private:
    targeted_skill subskill_;
    float speed_, v_ratio_, h_ratio_;
    math::vector2f projectile_offset_;
    character_base* caster_;

public:
    projectile(float speed);
    virtual ~projectile();
    void set_skill(const targeted_skill& skill);
    void set_speed_ratios(float h_ratio, float v_ratio);
    void update(float delta);

    void set_caster(character_base* caster) { caster_ = caster; }
    character_base* get_caster() const { return caster_; }
    const targeted_skill& get_subskill() const { return subskill_; }
};
}   // namespace encounter
} // namespace icarus
#endif // ICARUS_ENCOUNTER_PROJECTILE_HPP_
