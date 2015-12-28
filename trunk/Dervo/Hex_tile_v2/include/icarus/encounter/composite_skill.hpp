#ifndef ICARUS_ENCOUNTER_COMPOSITE_SKILL_HPP_
#define ICARUS_ENCOUNTER_COMPOSITE_SKILL_HPP_

#include <vector>

#include "icarus/encounter.hpp"
#include "icarus/utils/yth_handler.hpp"

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
    std::string display_name_;
    std::string animation_;
    float cast_time_;
    float cooldown_;
    float current_cooldown_;
    bool needs_target_;
    unsigned range_;
    int ammunition_cost_;
    std::vector<targeted_skill> subskills_;
    bool autocast_;
    uint16_t flags_;
    bool is_leveled_;
    bool is_ultimate_;
    unsigned level_;
public:
    composite_skill() : name_("unnamed"), display_name_("Unnamed"), cast_time_(0.0f),
                        cooldown_(0.0f), needs_target_(false), range_(0),
                        ammunition_cost_(0), autocast_(false), flags_(0x0) {}
    composite_skill(const std::string& name,
                    const std::string& display_name,
                    int ammunition,
                    uint16_t flags,
                    unsigned level = 0);
    virtual ~composite_skill();

    void add_subskill(targeted_skill subskill);
    void add_effect(applied_effect effect);
    void update_cooldown(double delta);
    void setup_from_data(utils::yth_node* skill_node);
    //void get_subskills(std::vector<targeted_skill>& subskills) const;

    const std::vector<targeted_skill>& get_subskills() { return subskills_; }
    float get_cast_time() const { return cast_time_; }
    bool is_ready() const { return (current_cooldown_ <= 0.0f); }
    const std::string& get_name() const { return name_; }
    const std::string& get_disp_name() const { return display_name_; }
    void apply_cooldown() { current_cooldown_ = cooldown_; }
    float get_cooldown() const { return cooldown_; }
    float current_cooldown() const { return current_cooldown_; }
    bool check_target_req() const { return needs_target_; }
    unsigned get_range() const { return range_; }
    void set_autocast(bool ac) { autocast_ = ac; }
    bool get_autocast() const { return autocast_; }
    uint16_t get_flags() const { return flags_; }
    const std::string& get_anim() const { return animation_; }
    unsigned get_level() const { return level_; }
    bool is_leveled() const { return is_leveled_; }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_COMPOSITE_SKILL_HPP_
