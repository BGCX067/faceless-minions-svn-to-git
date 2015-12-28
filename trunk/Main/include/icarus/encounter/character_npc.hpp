#ifndef ICARUS_ENCOUNTER_CHARACTER_NPC_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_NPC_HPP_

#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter/composite_skill.hpp"
#include "icarus/ai/behavior_node.hpp"
#include "icarus/encounter/data.hpp"

namespace icarus
{
namespace encounter
{
class grid_tile;
class character_npc : public character_base
{
protected:
    composite_skill skills_[4];
    unsigned skill_count_;
    ai::behavior_node* bt_root_;
    grid_tile* move_target_;
    int ranged_ammo_;
    int selected_skill_;
    int queued_skill_;
    grid_tile* skill_target_;
    grid_tile* melee_target_;
    unsigned experience_value_;

    // AI
    ai::node_result behavior_result_;

    // conditions
    bool check_if_off_screen();
    bool check_if_moving();
    bool check_if_casting();
    bool check_if_targets_in_range();
    bool check_los_to_target();
    bool check_remaining_ammo();
    bool check_targets_at_destination();

    // tasks
    ai::status::value move_into_screen();
    ai::status::value clear_path_nodes();
    ai::status::value select_skill();
    ai::status::value select_ammo_skill();
    ai::status::value select_target();
    ai::status::value use_skill();
    ai::status::value find_melee_target();
    ai::status::value move_into_range();

public:
    character_npc(grid* grid_ptr);
    virtual ~character_npc();

    virtual void update(double delta);
    void setup_from_data(utils::yth_node* data);

    composite_skill* get_queued_skill();
    grid_tile* get_targeted_tile();
    void spend_ammo(unsigned amount) { ranged_ammo_ -= amount; }
    unsigned get_xp_value() const { return experience_value_; }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_NPC_HPP_
