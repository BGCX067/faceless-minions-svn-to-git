#ifndef ICARUS_ENCOUNTER_DATA_HPP_
#define ICARUS_ENCOUNTER_DATA_HPP_

#include <queue>
#include "icarus/state_data.hpp"
#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace hero_class
{
enum type
{
    warrior,
    rogue,
    mage
};
}
struct hero_data
{
    unsigned index_;
    std::string name_;
    std::string surname_;
    unsigned skill_levels_[4];
    bool autocast_[5];
    unsigned base_damage_;
    int health_;
    int current_health_;
    int strength_;
    int agility_;
    int intelligence_;
    int damage_done_;
    int damage_taken_;
    unsigned kills_;
    unsigned assists_;
    int largest_taken_;
    int largest_given_;
    int friendly_fire_;
    float move_speed_;
    hero_class::type class_;
};
struct data : public state_data
{
    data() : experience_value_(0), gold_reward_(0), force_multiplier_(1.0f) {}
    ~data() {}

    utils::yth_node* encounter_node_;
    unsigned current_scenario_;
    unsigned scenario_count_;
    int experience_value_;
    int gold_reward_;
    float force_multiplier_;
    std::vector<hero_data> player_party_;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_DATA_HPP_
