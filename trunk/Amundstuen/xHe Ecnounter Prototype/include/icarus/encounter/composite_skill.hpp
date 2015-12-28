#ifndef ICARUS_ENCOUNTER_COMPOSITE_SKILL_HPP_
#define ICARUS_ENCOUNTER_COMPOSITE_SKILL_HPP_

#include <vector>

#include "icarus/encounter.hpp"

namespace icarus
{
namespace encounter
{
struct skill_data
{
    float cast_time_;
    std::string name_;
    float cooldown_;
    bool needs_target_;
    unsigned range_;
    std::vector<targeted_skill> subskills_;
};
class composite_skill
{
private:
    std::string name_;
    float cast_time_;
    float cooldown_;
    float current_cooldown_;
    bool needs_target_;
    unsigned range_;
    std::vector<targeted_skill> subskills_;
public:
    composite_skill() : name_("unnamed"), cast_time_(0.0f),
                        cooldown_(0.0f), needs_target_(false), range_(0) {}
    composite_skill(std::string name, float cast_time, float cooldown, int range);
    ~composite_skill();

    void add_subskill(targeted_skill subskill);
    void add_effect(applied_effect effect);
    void update_cooldown(float delta);
    //void get_subskills(std::vector<targeted_skill>& subskills) const;

    const std::vector<targeted_skill>& get_subskills() { return subskills_; }
    float get_cast_time() const { return cast_time_; }
    bool is_ready() const { return (current_cooldown_ <= 0.0f); }
    const std::string& get_name() const { return name_; }
    void apply_cooldown() { current_cooldown_ = cooldown_; }
    bool check_target_req() const { return needs_target_; }
    unsigned get_range() const { return range_; }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_COMPOSITE_SKILL_HPP_
