#ifndef ICARUS_ENCOUNTER_SKILL_HANDLER_HPP_
#define ICARUS_ENCOUNTER_SKILL_HANDLER_HPP_

#include <vector>

#include "icarus/encounter.hpp"
#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter/composite_skill.hpp"
#include "icarus/encounter/grid.hpp"
#include "icarus/encounter/projectile.hpp"
#include "icarus/encounter/visual_effect.hpp"

namespace icarus
{
namespace encounter
{
class skill_handler : public sf::Drawable
{
private:
    struct skill_cast
    {
        composite_skill* skill_;
        float time_remaining_;
        character_base* caster_;
        grid_tile* target_;
        skill_cast(composite_skill* skill,
                   character_base* caster,
                   grid_tile* target)
        :
            skill_(skill),
            caster_(caster),
            target_(target)
        {
            time_remaining_ = skill->get_cast_time();
        }
    };
    std::vector<skill_cast> active_casts_;
    std::vector<projectile> projectiles_;
    std::vector<visual_effect> effects_;
    grid* grid_;

    void cast_skill(const skill_cast& skill);
    void apply_subskill(targeted_skill& subskill,
                        const math::vector2i& vec_pos,
                        character_base* caster);
    void launch_projectile(const targeted_skill& subskill,
                           float angle,
                           character_base* caster);
    void add_effect(math::vector2f position,
                    utils::yth_node* effect);

public:
    skill_handler(grid* grid);
    ~skill_handler();

    void update();
    void add_cast(character_base* caster,
                  composite_skill* skill,
                  grid_tile* target = NULL);
    void interrupt(character_base* caster);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_SKILL_HANDLER_HPP_
