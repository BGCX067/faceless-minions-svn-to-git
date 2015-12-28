#ifndef ICARUS_ENCOUNTER_PROJECTILE_HPP_
#define ICARUS_ENCOUNTER_PROJECTILE_HPP_

#include <vector>
#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter.hpp"
#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
namespace encounter
{
class projectile : public animate
{
private:
    targeted_skill subskill_;
    float speed_, v_ratio_, h_ratio_;
    float angle_;
    math::vector2f projectile_offset_;
    character_base* caster_;
    std::vector<character_base*> missed_targets_;

public:
    projectile(float speed, utils::yth_node* data_node);
    virtual ~projectile();
    void set_skill(const targeted_skill& skill);
    void set_speed_ratios(float angle);
    void update(float delta);

    void set_caster(character_base* caster) { caster_ = caster; }
    character_base* get_caster() const { return caster_; }
    targeted_skill& get_subskill() { return subskill_; }
    void add_miss(character_base* target) { missed_targets_.push_back(target); }
    bool has_missed_target(character_base* target) const
    {
        for (unsigned i = 0; i < missed_targets_.size(); ++i)
            if (target == missed_targets_[i])
                return true;
        return false;
    }
};
}   // namespace encounter
} // namespace icarus
#endif // ICARUS_ENCOUNTER_PROJECTILE_HPP_
