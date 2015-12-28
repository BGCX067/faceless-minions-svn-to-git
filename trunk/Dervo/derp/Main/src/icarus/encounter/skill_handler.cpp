#include "icarus/encounter/skill_handler.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/encounter/character_npc.hpp"
#include "icarus/encounter/character_hero.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/sound.hpp"

namespace icarus
{
namespace encounter
{
skill_handler::skill_handler(grid* grid, ui::skillbar* bar)
:
    grid_(grid),
    skill_bar_(bar)
{
}
skill_handler::~skill_handler()
{
}

void skill_handler::cast_skill(const skill_cast& cast)
{
    utils::yth_node* skill_node = resource_handler::get()->get_root_node("skills")->child(cast.skill_->get_name());
    if (skill_node->child_count("sfx") > 0)
        sound::get()->play_sfx(skill_node->child("sfx")->value());
    cast.caster_->set_casting(false);
    cast.skill_->apply_cooldown();
    if (cast.skill_->get_anim() != "")
    {
        cast.caster_->set_animation_state(cast.skill_->get_anim(), true);
        if (cast.skill_->get_range() > 0)
            cast.caster_->orient_sprite(cast.caster_->get_world_pos().x < cast.target_->get_world_pos().x);
    }
    if (cast.index_ != -1 && cast.caster_ == dynamic_cast<character_base*>(skill_bar_->get_selected_hero()))
        skill_bar_->set_cooldown(cast.index_, cast.skill_->get_cooldown());

    if (cast.skill_->get_range() > 3 && cast.caster_->is_ai())
        dynamic_cast<character_npc*>(cast.caster_)->spend_ammo(1);
    // cast the damn skill
    std::vector<targeted_skill> subskills = cast.skill_->get_subskills();
    for (unsigned i = 0; i < subskills.size(); ++i)
    {
        if (subskills[i].flags & skill_flags::USES_WEAPON_DMG)
            subskills[i].damage += cast.caster_->get_weapon_dmg();

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
        case skill_type::railgun:
            launch_projectile(subskills[i],
                              cast.caster_->get_world_pos().get_angle_to(cast.target_->get_world_pos()),
                              cast.caster_,
                              true);
            break;
        case skill_type::self:
            apply_subskill(subskills[i], cast.caster_->get_vector_pos(), cast.caster_, true);
            break;
        default:
            break;
        }
    }
}

void skill_handler::apply_subskill(targeted_skill& subskill,
                                   const math::vector2i& vec_pos,
                                   character_base* caster,
                                   bool self_buff)
{
    std::vector<grid_tile*> affected_tiles;
    subskill.set_source(caster);
    grid_->get_tiles_in_radius(vec_pos,
                               subskill.radius,
                               affected_tiles);
    bool selfcast = (caster->get_vector_pos() == vec_pos);
    for (unsigned k = 0; k < affected_tiles.size(); ++k)
    {
        character_base* const occupant = affected_tiles[k]->get_occupant();
        if (occupant != NULL && !(selfcast && caster == occupant && !self_buff))
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
                                      character_base* caster,
                                      bool railgun)
{
    utils::yth_node* projectile_data = resource_handler::get()->get_root_node("effects")->child("projectiles")->child(subskill.projectile);
    if (railgun)
    {
        projectile new_projectile(225.0f, projectile_data, true, subskill.range);
        new_projectile.set_skill(subskill);
        new_projectile.set_speed_ratios(angle);
        new_projectile.set_initial_pos(caster->get_world_pos());
        new_projectile.set_caster(caster);
        projectiles_.push_back(new_projectile);
    }
    else
    {
        projectile new_projectile(700.0f, projectile_data);
        new_projectile.set_skill(subskill);
        new_projectile.set_speed_ratios(angle);
        new_projectile.set_initial_pos(caster->get_world_pos());
        new_projectile.set_caster(caster);
        projectiles_.push_back(new_projectile);
    }
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
            if (active_casts_[i].caster_->is_incapacitated())
                interrupt(active_casts_[i].caster_);
            active_casts_[i].time_remaining_ -= delta;
            active_casts_[i].caster_->update_cast_bar(active_casts_[i].time_remaining_,
                                                      active_casts_[i].skill_->get_cast_time());
        }
    }
    for (int i = 0; i < static_cast<int>(projectiles_.size()); ++i)
    {
        projectiles_[i].update(delta);
        grid_tile* current_tile = grid_->get_tile_at(projectiles_[i].get_position());
        if (current_tile)
        {
            if (current_tile->get_occupant() &&
                current_tile->get_occupant() != projectiles_[i].get_caster() &&
                !projectiles_[i].is_railgun())
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
                        add_effect(projectiles_[i].get_projectile_pos(),
                                   resource_handler::get()->get_root_node("effects")->
                                   child("projectiles")->child(applied_skill.projectile)->
                                   child("impact"));

                        if (resource_handler::get()->get_root_node("effects")->
                                   child("projectiles")->child(applied_skill.projectile)->
                                   child("impact")->child_count("sfx"))
                            sound::get()->play_sfx(resource_handler::get()->get_root_node("effects")->
                                    child("projectiles")->child(applied_skill.projectile)->
                                    child("impact")->child("sfx")->value());
                    }
                    projectiles_.erase(projectiles_.begin() + i);
                    i--;
                    continue;
                }
            }
            else if (projectiles_[i].is_railgun())
            {
                if (projectiles_[i].tile_already_affected(current_tile->get_vector_pos()))
                {
                    continue;
                }
                else if (current_tile->get_vector_pos() != projectiles_[i].get_caster()->get_vector_pos())
                {
                    projectiles_[i].add_affected_tile(current_tile->get_vector_pos());
                    targeted_skill applied_skill = projectiles_[i].get_subskill();
                    if (current_tile->get_occupant() &&
                        current_tile->get_occupant() != projectiles_[i].get_caster())
                    {
                        apply_subskill(applied_skill,
                                   current_tile->get_vector_pos(),
                                   projectiles_[i].get_caster());
                    }
                    add_effect(current_tile->get_world_pos(),
                                   resource_handler::get()->get_root_node("effects")->
                                   child("projectiles")->child(applied_skill.projectile));
                    if (projectiles_[i].range_left() == 0)
                    {
                        projectiles_.erase(projectiles_.begin() + i);
                        i--;
                        continue;
                    }
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
                             int index,
                             composite_skill* skill,
                             grid_tile* target)
{
    if (caster->is_casting())
        interrupt(caster);
    if (caster->is_incapacitated())
        return;
    skill_cast new_cast(skill, index, caster, target);
    active_casts_.push_back(new_cast);
    caster->set_casting(true);
    caster->set_show_castbar((skill->get_cast_time() > 0.5f));
    if (skill->get_flags() & skill_flags::IS_MAGIC &&
        !caster->is_moving())
    {
        caster->set_animation_state("channeling", true);
        if (skill->get_range() > 0)
            caster->orient_sprite(caster->get_world_pos().x < target->get_world_pos().x);
    }
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
