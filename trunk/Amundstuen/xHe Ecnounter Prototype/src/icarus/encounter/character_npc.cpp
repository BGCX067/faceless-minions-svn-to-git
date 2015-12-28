#include "icarus/encounter/character_npc.hpp"

#include "icarus/ai/selector_node.hpp"
#include "icarus/ai/sequence_node.hpp"
#include "icarus/ai/parallel_node.hpp"
#include "icarus/ai/debug_node.hpp"
#include "icarus/ai/timer_node.hpp"
#include "icarus/encounter/ai/condition.hpp"
#include "icarus/encounter/ai/task.hpp"
#include "icarus/encounter/grid.hpp"

namespace icarus
{
namespace encounter
{
character_npc::character_npc(grid* grid_ptr)
:
    character_base(grid_ptr, "unnamed"),
    bt_root_(NULL),
    move_target_(NULL)
{
    hp_bar_.set_color(sf::Color::Red);

    bt_root_ = new ai::selector_node(false);
    bt_root_->
    add(new ai::sequence_node())->
        add(new ai::encounter::condition(*this, &character_npc::check_for_path))->end()->
        add(new ai::encounter::condition(*this, &character_npc::check_path_end))->end()->
        add(new ai::encounter::task(*this, &character_npc::move_along_path))->end()->
    end()->
    add(new ai::sequence_node())->
        add(new ai::encounter::task(*this, &character_npc::get_potential_target))->end()->
        add(new ai::encounter::condition(*this, &character_npc::check_potential_target))->end()->
        add(new ai::encounter::task(*this, &character_npc::set_path));
    //ctor
}

character_npc::~character_npc()
{
    //dtor
}

bool character_npc::check_for_path()
{
    if (path_to_target_.size() > 0)
        return true;
    return false;
}

bool character_npc::check_path_end()
{
    if (grid_ptr_->get_tile_at(path_to_target_[0]->vec_pos)->get_state()
        == tile_state::unoccupied)
        return true;
    return false;
}

bool character_npc::check_potential_target()
{
    if (move_target_->get_state()
        == tile_state::unoccupied)
        return true;
    return false;
}

ai::status::value character_npc::get_potential_target()
{
    move_target_ = grid_ptr_->get_random_tile();
    if (move_target_ != NULL)
        return ai::status::success;
    return ai::status::error;
}

ai::status::value character_npc::set_path()
{
    grid_ptr_->get_path_to_tile(move_target_->get_vector_pos(), this);
    return ai::status::success;
}

ai::status::value character_npc::move_along_path()
{
    if (this->get_world_pos() != this->get_target_pos())
    {
        update_movement();
        return ai::status::running;
    }
    else
    {
        update_movement();
        return ai::status::success;
    }
}

void character_npc::update(float delta)
{
    for (unsigned i = 0; i < 4; ++i)
        skills_[i].update_cooldown(delta);

    for (unsigned i = 0; i < effects_.size(); ++i)
    {
        effects_[i].time_left -= delta;
        if (effects_[i].time_left <= effects_[i].tick_step *
                                     (effects_[i].ticks_remaining - 1))
        {
            effects_[i].ticks_remaining--;
            apply_effect(effects_[i].effect);
        }
        if (effects_[i].time_left <= 0.0f)
            effects_.erase(effects_.begin() + i);
    }
    combat_text_.update(delta);
    hp_bar_.update_bar(delta);
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.6f);
    hp_bar_.set_position(pos);

    bt_root_->execute();
}
void character_npc::setup_from_data(npc_data* data)
{
    move_speed_ = data->move_speed_;
    health_ = data->health_;
    current_health_ = health_;
    name_ = data->name_;

    for (unsigned i = 0; i < data->skill_count_; ++i)
    {
        composite_skill temp(data->skills_[i].name_,
                             data->skills_[i].cast_time_,
                             data->skills_[i].cooldown_,
                             data->skills_[i].range_);
        for (unsigned k = 0; k < data->skills_[i].subskills_.size(); ++k)
        {
            temp.add_subskill(data->skills_[i].subskills_[k]);
        }
        skills_[i] = temp;
    }
}
}   // namespace encounter
}   // namespace icarus
