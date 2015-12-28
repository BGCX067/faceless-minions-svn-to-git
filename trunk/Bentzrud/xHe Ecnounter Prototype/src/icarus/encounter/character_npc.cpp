#include "icarus/encounter/character_npc.hpp"

#include "icarus/ai/selector_node.hpp"
#include "icarus/ai/sequence_node.hpp"
#include "icarus/ai/parallel_node.hpp"
#include "icarus/ai/forced_result_node.hpp"
#include "icarus/ai/loop_node.hpp"
#include "icarus/ai/reversal_node.hpp"
#include "icarus/encounter/ai/condition.hpp"
#include "icarus/encounter/ai/task.hpp"
#include "icarus/encounter/grid.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
character_npc::character_npc(grid* grid_ptr)
:
    character_base(grid_ptr, "unnamed"),
    bt_root_(NULL),
    move_target_(NULL),
    ranged_ammo_(0),
    selected_skill_(-1),
    queued_skill_(-1),
    skill_target_(NULL),
    melee_target_(NULL)
{
    hp_bar_.set_color(utils::rgb(0xBA4343));
    is_ai_ = true;

    bt_root_ = new ai::selector_node(false);
    bt_root_->add(new ai::sequence_node())->
        add(new ai::encounter::condition(*this, &character_npc::check_if_off_screen))->end()->
        add(new ai::reversal_node())->
            add(new ai::encounter::condition(*this, &character_npc::check_if_moving))->end()->
        end()->
        add(new ai::encounter::task(*this, &character_npc::move_into_screen))->end()->
    end()->
    add(new ai::selector_node(false))->
        add(new ai::encounter::condition(*this, &character_npc::check_if_off_screen))->end()->
        add(new ai::encounter::condition(*this, &character_npc::check_if_moving))->end()->
        add(new ai::encounter::condition(*this, &character_npc::check_if_casting))->end()->
    end()->
    add(new ai::sequence_node())->
        add(new ai::encounter::condition(*this, &character_npc::check_remaining_ammo))->end()->
        add(new ai::forced_result_node(ai::status::success))->
            add(new ai::sequence_node())->
                add(new ai::encounter::task(*this, &character_npc::select_skill))->end()->
                add(new ai::loop_node(5, ai::status::success))->
                    add(new ai::parallel_node(true, false))->
                        add(new ai::encounter::task(*this, &character_npc::select_target))->end()->
                        add(new ai::encounter::condition(*this, &character_npc::check_los_to_target))->end()->
                    end()->
                end()->
                add(new ai::encounter::task(*this, &character_npc::use_skill))->end()->
            end()->
        end()->
    end()->
    add(new ai::selector_node(false))->
        add(new ai::sequence_node())->
            add(new ai::encounter::task(*this, &character_npc::select_skill))->end()->
            add(new ai::encounter::condition(*this, &character_npc::check_if_targets_in_range))->end()->
            add(new ai::reversal_node())->
                add(new ai::parallel_node(true, false))->
                    add(new ai::encounter::condition(*this, &character_npc::check_if_moving))->end()->
                    add(new ai::encounter::task(*this, &character_npc::clear_path_nodes))->end()->
                end()->
            end()->
            add(new ai::encounter::task(*this, &character_npc::select_target))->end()->
            add(new ai::encounter::task(*this, &character_npc::use_skill))->end()->
        end()->
        add(new ai::selector_node(false))->
            add(new ai::encounter::condition(*this, &character_npc::check_if_moving))->end()->
            add(new ai::sequence_node())->
                add(new ai::encounter::condition(*this, &character_npc::check_if_targets_in_range))->end()->
                add(new ai::encounter::task(*this, &character_npc::clear_path_nodes))->end()->
            end()->
            add(new ai::sequence_node())->
                add(new ai::encounter::task(*this, &character_npc::find_melee_target))->end()->
                add(new ai::encounter::task(*this, &character_npc::move_into_range));
    //ctor
}

character_npc::~character_npc()
{
    if (bt_root_)
        delete bt_root_;
}

void character_npc::update(double delta)
{
    combat_text_.update(delta);
    hp_bar_.update_bar(delta);
    update_movement();
    if (is_dead_)
    {
        return;
    }
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

    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.6f);
    hp_bar_.set_position(pos);

    bt_root_->execute();
}
void character_npc::setup_from_data(utils::yth_node* data)
{
    move_speed_ = atoi(data->child("base_speed")->value().c_str());
    health_ = atoi(data->child("health")->value().c_str());
    current_health_ = health_;
    name_ = atoi(data->child("display_name")->value().c_str());
    ranged_ammo_ = atoi(data->child("ammunition")->value().c_str());

    utils::yth_node* skill_node = resource_handler::get()->get_root_node("skills");

    for (unsigned i = 0; i < data->child_count("skill"); ++i)
    {
        composite_skill temp(data->child("skill", i)->value(),
                             skill_node->child(data->child("skill", i)->value())->child("display_name")->value(),
                             atof(skill_node->child(data->child("skill", i)->value())->child("base_cast_time")->value().c_str()),
                             atof(skill_node->child(data->child("skill", i)->value())->child("base_cooldown")->value().c_str()),
                             atoi(skill_node->child(data->child("skill", i)->value())->child("range")->value().c_str()),
                             atoi(skill_node->child(data->child("skill", i)->value())->child("ammunition_cost")->value().c_str()),
                             atoi(skill_node->child(data->child("skill", i)->value())->child("flags")->value().c_str()));
        temp.setup_from_data(skill_node->child(data->child("skill", i)->value()));
        skills_[i] = temp;
    }

    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets");
    std::string sheet_path = sprite_node->child(data->child("sprite_sheet")->value())->child("path")->value();
    utils::yth_node* animation_node = sprite_node->child(data->child("sprite_sheet")->value())->child(data->key());
    load_sprite(animation_node, sheet_path);
}

composite_skill* character_npc::get_queued_skill()
{
    if (queued_skill_ != -1)
    {
        composite_skill* skill = &skills_[queued_skill_];
        queued_skill_ = -1;
        return skill;
    }
    return NULL;
}

grid_tile* character_npc::get_targeted_tile()
{
    if (skill_target_)
    {
        grid_tile* tile = skill_target_;
        skill_target_ = NULL;
        return tile;
    }
    return NULL;
}

// AI FUNCTIONS ----------
// conditions
bool character_npc::check_if_off_screen()
{
    return (vector_position_.x >= (vector_position_.y == 0 ? 23 : 23 - floor(vector_position_.y * 0.5)));
}
bool character_npc::check_if_moving()
{
    return is_moving_;
}
bool character_npc::check_if_casting()
{
    return is_casting_;
}
bool character_npc::check_if_targets_in_range()
{
    if (selected_skill_ != -1 || ranged_ammo_ <= 0)
    {
        std::vector<grid_tile*> tiles;
        int range = (ranged_ammo_ <= 0 ? 1 : skills_[selected_skill_].get_range());
        grid_ptr_->get_tiles_in_radius(vector_position_,
                                       range,
                                       tiles);
        for (unsigned i = 0; i < tiles.size(); ++i)
        {
            if (tiles[i]->get_occupant() != NULL &&
                 !tiles[i]->get_occupant()->is_ai())
            {
                return true;
            }
        }
    }
    return false;
}
bool character_npc::check_los_to_target()
{
    if (!skill_target_)
        return false;

    float angle = this->get_world_pos().get_angle_to(skill_target_->get_world_pos());
    float h_ratio = sin(angle);
    float v_ratio = cos(angle);
    math::vector2f offset(h_ratio, v_ratio);
    math::vector2f pos = this->get_world_pos();
    grid_tile* current_tile = NULL;
    while (current_tile != skill_target_)
    {
        pos += offset;
        current_tile = grid_ptr_->get_tile_at(pos);
        if (current_tile != NULL &&
            current_tile->get_occupant() != this &&
            current_tile->get_occupant() != NULL &&
            current_tile->get_occupant()->is_ai())
            return false;
    }
    return true;
}
bool character_npc::check_remaining_ammo()
{
    return ranged_ammo_;
}
bool character_npc::check_path_to_target()
{
    // TODO:: is this needed?
    return true;
}

// tasks
ai::status::value character_npc::move_into_screen()
{
    if (grid_ptr_->get_path_to_tile(vector_position_ - math::vector2i(2, 0), this))
        return ai::status::success;
    return ai::status::failed;
}
ai::status::value character_npc::clear_path_nodes()
{
    clear_path();
    return ai::status::success;
}
ai::status::value character_npc::select_skill()
{
    if (skills_[0].is_ready() && ranged_ammo_ > 0)
        selected_skill_ = 0;
    else if (skills_[1].is_ready())
        selected_skill_ = 1;

    return (selected_skill_ != -1 ? ai::status::success : ai::status::failed);
}
ai::status::value character_npc::select_target()
{
    if (selected_skill_ != -1)
    {
        std::vector<grid_tile*> tiles;
        grid_ptr_->get_tiles_in_radius(vector_position_,
                                       skills_[selected_skill_].get_range(),
                                       tiles);
        int range = -1;
        int index = -1;
        for (unsigned i = 0; i < tiles.size(); ++i)
        {
            if ((range == -1 || tiles[i]->get_range_from(vector_position_) < static_cast<unsigned>(range)) &&
                (tiles[i]->get_occupant() != NULL && !tiles[i]->get_occupant()->is_ai()))
            {
                range = tiles[i]->get_range_from(vector_position_);
                index = i;
            }
        }
        if (index != -1)
        {
            skill_target_ = tiles[index];
            return ai::status::success;
        }
    }
    return ai::status::failed;
}
ai::status::value character_npc::use_skill()
{
    if (selected_skill_ != -1 && skill_target_ != NULL)
    {
        queued_skill_ = selected_skill_;
        selected_skill_ = -1;
        return ai::status::success;
    }
    return ai::status::failed;
}
ai::status::value character_npc::find_melee_target()
{
    std::vector<grid_tile*> tiles;
    grid_ptr_->get_tiles_in_radius(vector_position_,
                                   30,
                                   tiles);
    int range = -1;
    int index = -1;
    for (unsigned i = 0; i < tiles.size(); ++i)
    {
        if ((range == -1 || tiles[i]->get_range_from(vector_position_) < static_cast<unsigned>(range)) &&
            (tiles[i]->get_occupant() != NULL && !tiles[i]->get_occupant()->is_ai()))
        {
            range = tiles[i]->get_range_from(vector_position_);
            index = i;
        }
    }
    if (index != -1)
    {
        melee_target_ = tiles[index];
        return ai::status::success;
    }
    return ai::status::failed;
}
ai::status::value character_npc::move_into_range()
{
    if (melee_target_ != NULL &&
        melee_target_->get_occupant() != NULL &&
        melee_target_->get_occupant()->is_ai() == false)
    {
        std::vector<grid_tile*> tiles;
        grid_ptr_->get_tiles_in_radius(melee_target_->get_vector_pos(),
                                       1,
                                       tiles);
        int range = -1;
        int index = -1;
        for (unsigned i = 0; i < tiles.size(); ++i)
        {
            if ((range == -1 || tiles[i]->get_range_from(melee_target_->get_vector_pos()) < static_cast<unsigned>(range)) &&
                (tiles[i]->get_state() == tile_state::unoccupied || tiles[i]->get_state() == tile_state::out_of_bounds))
            {
                range = tiles[i]->get_range_from(melee_target_->get_vector_pos());
                index = i;
            }
        }
        if (index != -1)
        {
            grid_ptr_->get_path_to_tile(tiles[index]->get_vector_pos(), this);
            return ai::status::success;
        }
    }
    return ai::status::failed;
}
// END AI FUNCTIONS ------
}   // namespace encounter
}   // namespace icarus
