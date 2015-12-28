#ifndef ICARUS_ENCOUNTER_CHARACTER_NPC_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_NPC_HPP_

#include "character_base.hpp"
#include "composite_skill.hpp"
#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace encounter
{
class grid_tile;
struct npc_data
{
    std::string name_;
    skill_data skills_[4];
    unsigned skill_count_;
    int health_;
    float move_speed_;
    std::string sprite_;
};
class character_npc : public character_base
{
protected:
    composite_skill skills_[4];
    unsigned skill_count_;
    ai::behavior_node* bt_root_;
    grid_tile* move_target_;

    bool check_for_path();
    bool check_path_end();
    bool check_potential_target();
    ai::status::value get_potential_target();
    ai::status::value set_path();
    ai::status::value move_along_path();

public:
    character_npc(grid* grid_ptr);
    virtual ~character_npc();

    virtual void update(float delta);
    void setup_from_data(npc_data* data);
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_NPC_HPP_
