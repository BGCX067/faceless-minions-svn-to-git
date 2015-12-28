#include "icarus/encounter/skill_handler.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/encounter/character_npc.hpp"
#include "icarus/state_manager.hpp"

namespace icarus
{
namespace encounter
{
skill_handler::skill_handler(grid* grid)
:
    grid_(grid)
{
}
skill_handler::~skill_handler()
{
}

void skill_handler::cast_skill(const skill_cast& cast)
{
    cast.caster_->set_casting(false);
    cast.skill_->apply_cooldown();

    if (cast.skill_->get_range() > 3 && cast.caster_->is_ai())
        dynamic_cast<character_npc*>(cast.caster_)->spend_ammo(1);
    // cast the damn skill
    std::vector<targeted_skill> subskills = cast.skill_->get_subskills();
    for (unsigned i = 0; i < subskills.size(); ++i)
    {
        switch (subskills[i].type)
        {
        case skill_type::instant:
            apply_subskill(subskills[i], cast.target_->get_vector_pos(), cast.caster_);
            break;
        case skill_type::projectile:
            launch_projectile(subskills[i],
                              cast.caster_->get_world_pos().get_angle_to(cast.target_->get_world_pos()),
                              cast.caster_);
            break;
        case skill_type::beam:

            break;
        default:
            break;
        }
    }
}

void skill_handler::apply_subskill(targeted_skill& subskill,
                                   const math::vector2i& vec_pos,
                                   character_base* caster)
{
    std::vector<grid_tile*> affected_tiles;
    grid_->get_tiles_in_radius(vec_pos,
                               subskill.radius,
                               affected_tiles);
    for (unsigned k = 0; k < affected_tiles.size(); ++k)
    {
        character_base* const occupant = affected_tiles[k]->get_occupant();
        if (occupant != NULL)
        {
            // affect occupant here
            targeted_skill applied_skill = subskill;
            if (subskill.flags & skill_flags::IS_MAGIC)
                applied_skill.damage *= caster->get_magical_mod();
            else
                applied_skill.damage *= caster->get_physical_mod();

            if (applied_skill.effect.flags & skill_flags::IS_MAGIC)
                applied_skill.effect.damage *= caster->get_magical_mod();
            else
                applied_skill.effect.damage *= caster->get_physical_mod();

            if (static_cast<unsigned>(rand()%100) < caster->get_crit_chance())
            {
                applied_skill.damage *= caster->get_crit_mod();
                applied_skill.flags |= skill_flags::IS_CRITICAL;
            }
            occupant->apply_skill(applied_skill);
        }
    }
}

void skill_handler::launch_projectile(const targeted_skill& subskill,
                                      float angle,
                                      character_base* caster)
{
    utils::yth_node* projectile_data = resource_handler::get()->get_root_node("effects")->child("projectiles")->child(subskill.projectile);
    projectile new_projectile(700.0f, projectile_data);
    new_projectile.set_skill(subskill);
    new_projectile.set_speed_ratios(angle);
    new_projectile.update_pos(caster->get_world_pos(), caster->get_vector_pos());
    new_projectile.set_caster(caster);
    projectiles_.push_back(new_projectile);
}

void skill_handler::update()
{
    double delta = state_manager::get()->get_delta();
    for (unsigned i = 0; i < active_casts_.size(); ++i)
    {
        if (active_casts_[i].time_remaining_ <= 0.0f)
        {
            cast_skill(active_casts_[i]);
            active_casts_.erase(active_casts_.begin() + i);
        }
        else
        {
            active_casts_[i].time_remaining_ -= delta;
            active_casts_[i].caster_->update_cast_bar(active_casts_[i].time_remaining_,
                                                      active_casts_[i].skill_->get_cast_time());
        }
    }
    for (int i = 0; i < static_cast<int>(projectiles_.size()); ++i)
    {
        projectiles_[i].update(delta);
        grid_tile* current_tile = grid_->get_tile_at(projectiles_[i].get_world_pos());
        if (current_tile)
        {
            if (current_tile->get_occupant() &&
                current_tile->get_occupant() != projectiles_[i].get_caster())
            {
                if (projectiles_[i].has_missed_target(current_tile->get_occupant()))
                    continue;

                if (projectiles_[i].get_subskill().flags & skill_flags::CAN_DODGE &&
                    static_cast<unsigned>(rand()%100) < current_tile->get_occupant()->get_dodge())
                {
                    bool crit = static_cast<unsigned>(rand()%100) < projectiles_[i].get_caster()->get_crit_chance();
                    current_tile->get_occupant()->print_dodge(crit);
                    projectiles_[i].add_miss(current_tile->get_occupant());
                }
                else
                {
                    targeted_skill applied_skill = projectiles_[i].get_subskill();
                    applied_skill.flags &= ~skill_flags::CAN_DODGE;
                    apply_subskill(applied_skill,
                               current_tile->get_vector_pos(),
                               projectiles_[i].get_caster());
                    if (resource_handler::get()->get_root_node("effects")->
                        child("projectiles")->child(applied_skill.projectile)->
                        child_count("impact"))
                    {
                        add_effect(projectiles_[i].get_world_pos(),
                                   resource_handler::get()->get_root_node("effects")->
                                   child("projectiles")->child(applied_skill.projectile)->
                                   child("impact"));
                    }
                    projectiles_.erase(projectiles_.begin() + i);
                    i--;
                    continue;
                }
            }
        }
        else
        {
            projectiles_.erase(projectiles_.begin() + i);
            i--;
            continue;
        }
    }
    for (int i = 0; i < static_cast<int>(effects_.size()); ++i)
    {
        if (effects_[i].is_active())
        {
            effects_[i].update(delta);
        }
        else
        {
            effects_.erase(effects_.begin() + i);
            i--;
            continue;
        }
    }
}

void skill_handler::add_cast(character_base* caster,
                             composite_skill* skill,
                             grid_tile* target)
{
    if (caster->is_casting())
        interrupt(caster);
    skill_cast new_cast(skill, caster, target);
    active_casts_.push_back(new_cast);
    caster->set_casting(true);
}

void skill_handler::interrupt(character_base* caster)
{
    for (unsigned i = 0; i < active_casts_.size(); ++i)
    {
        if (active_casts_[i].caster_ == caster)
        {
            active_casts_.erase(active_casts_.begin() + i);
            caster->set_casting(false);
        }
    }
}

void skill_handler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (unsigned i = 0; i < projectiles_.size(); ++i)
    {
        projectiles_[i].draw(target, states);
    }
    for (unsigned i = 0; i < effects_.size(); ++i)
    {
        effects_[i].draw(target, states);
    }
}

void skill_handler::add_effect(math::vector2f position,
                               utils::yth_node* effect)
{
    visual_effect temp(position, effect, false);
    effects_.push_back(temp);
}
}   // namespace encounter
}   // namespace icarus
