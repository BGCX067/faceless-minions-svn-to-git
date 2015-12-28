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
    {
        sound::get()->play_random_sfx(skill_node->child("sfx")->value());
    }
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

    if (cast.skill_->get_ammo_cost() > 0 && cast.caster_->is_ai())
        dynamic_cast<character_npc*>(cast.caster_)->spend_ammo(cast.skill_->get_ammo_cost());
    // cast the damn skill
    std::vector<targeted_skill> subskills = cast.skill_->get_subskills();
    for (unsigned i = 0; i < subskills.size(); ++i)
    {
        if (subskills[i].flags & skill_flags::USES_WEAPON_DMG)
            subskills[i].damage += cast.caster_->get_weapon_dmg();

        grid_tile* target = (subskills[i].range == 0 ? grid_->get_tile_at(cast.caster_->get_vector_pos()) :
                                                        cast.target_);

        if (subskills[i].delay > 0)
        {
            delayed_cast temp;
            temp.time_remaining_ = subskills[i].delay;
            temp.skill_ = subskills[i];
            temp.caster_ = cast.caster_;
            temp.target_ = target;
            delayed_.push_back(temp);
            continue;
        }
        else if (subskills[i].channeled_duration > 0)
        {
            channeled_cast temp;
            temp.time_remaining_ = subskills[i].channeled_duration;
            temp.interval_ = subskills[i].channeled_delay;
            temp.current_interval_ = temp.interval_;
            temp.interval_reduction_ = subskills[i].channeled_delay_mod;
            temp.skill_ = subskills[i];
            temp.caster_ = cast.caster_;
            temp.target_ = target;
            channeled_.push_back(temp);
        }

        if (subskills[i].hide_character)
            cast.caster_->set_invisible(true);

        if (subskills[i].sfx != "")
            sound::get()->play_random_sfx(subskills[i].sfx);

        switch (subskills[i].type)
        {
        case skill_type::instant:
            apply_subskill(subskills[i], target->get_vector_pos(), cast.caster_);
            break;
        case skill_type::projectile:
            launch_projectile(subskills[i],
                              cast.caster_->get_world_pos().get_angle_to(target->get_world_pos()),
                              cast.caster_);
            break;
        case skill_type::railgun:
            launch_projectile(subskills[i],
                              cast.caster_->get_world_pos().get_angle_to(target->get_world_pos()),
                              cast.caster_,
                              true,
                              cast.target_);
            break;
        case skill_type::self:
            apply_subskill(subskills[i], cast.caster_->get_vector_pos(), cast.caster_, true);
            break;
        case skill_type::teleport:
            teleport(cast.caster_, grid_->get_tile_at(cast.caster_->get_vector_pos()),
                        target);
            break;
        case skill_type::self_aoe:
            apply_subskill(subskills[i], cast.caster_->get_vector_pos(), cast.caster_);
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
    if (subskill.visual_effect != "")
    {
        utils::yth_node* effect_node = resource_handler::get()->get_root_node("effects")->child("skill_effects")->child(subskill.visual_effect);
        add_effect(grid_->get_tile_at(vec_pos)->get_world_pos(), effect_node);
    }
}

void skill_handler::launch_projectile(const targeted_skill& subskill,
                                      float angle,
                                      character_base* caster,
                                      bool railgun,
                                      grid_tile* target)
{
    utils::yth_node* projectile_data = resource_handler::get()->get_root_node("effects")->child("projectiles")->child(subskill.projectile);
    float speed = (subskill.projectile_speed > 0.0f ? subskill.projectile_speed : 800.0f);
    if (railgun)
    {
        projectile new_projectile(speed, projectile_data, true);
        new_projectile.set_skill(subskill);
        new_projectile.set_speed_ratios(angle);
        new_projectile.set_initial_pos(caster->get_world_pos());
        new_projectile.set_caster(caster);
        new_projectile.setup_interval(subskill.projectile_interval);
        new_projectile.set_reverse((angle < 0 || angle > 180));
        new_projectile.set_target(target);
        projectiles_.push_back(new_projectile);
    }
    else
    {
        projectile new_projectile(speed, projectile_data);
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
                    if (projectiles_[i].get_subskill().hide_character)
                        projectiles_[i].get_caster()->set_invisible(false);
                    projectiles_.erase(projectiles_.begin() + i);
                    i--;
                    continue;
                }
            }
            else if (projectiles_[i].is_railgun())
            {
                if (projectiles_[i].update_interval(delta))
                {
                    add_effect(projectiles_[i].get_position(),
                               resource_handler::get()->get_root_node("effects")->
                               child("projectiles")->child(projectiles_[i].get_subskill().projectile),
                               projectiles_[i].get_reverse());
                }
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
                }
                if (current_tile == projectiles_[i].get_target())
                {
                    if (projectiles_[i].get_subskill().hide_character)
                        projectiles_[i].get_caster()->set_invisible(false);
                    projectiles_.erase(projectiles_.begin() + i);
                    i--;
                    continue;
                }
            }
        }
        else
        {
            if (projectiles_[i].get_subskill().hide_character)
                projectiles_[i].get_caster()->set_invisible(false);
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

    for (int i = 0; i < static_cast<int>(delayed_.size()); ++i)
    {
        delayed_[i].time_remaining_ -= delta;
        if (delayed_[i].time_remaining_ <= 0.0f)
        {
            if (delayed_[i].skill_.hide_character)
                delayed_[i].caster_->set_invisible(true);
            if (delayed_[i].skill_.sfx != "")
                sound::get()->play_random_sfx(delayed_[i].skill_.sfx );
            if (delayed_[i].skill_.animation != "")
                delayed_[i].caster_->set_animation_state(delayed_[i].skill_.animation, true);
            switch (delayed_[i].skill_.type)
            {
            case skill_type::instant:
                apply_subskill(delayed_[i].skill_, delayed_[i].target_->get_vector_pos(), delayed_[i].caster_);
                break;
            case skill_type::projectile:
                launch_projectile(delayed_[i].skill_,
                                  delayed_[i].caster_->get_world_pos().get_angle_to(delayed_[i].target_->get_world_pos()),
                                  delayed_[i].caster_);
                break;
            case skill_type::railgun:
                launch_projectile(delayed_[i].skill_,
                                  delayed_[i].caster_->get_world_pos().get_angle_to(delayed_[i].target_->get_world_pos()),
                                  delayed_[i].caster_,
                                  true,
                                  delayed_[i].target_);
                break;
            case skill_type::self:
                apply_subskill(delayed_[i].skill_, delayed_[i].caster_->get_vector_pos(), delayed_[i].caster_, true);
                break;
            case skill_type::teleport:
                teleport(delayed_[i].caster_, grid_->get_tile_at(delayed_[i].caster_->get_vector_pos()),
                        delayed_[i].target_);
                break;
            case skill_type::self_aoe:
                apply_subskill(delayed_[i].skill_, delayed_[i].caster_->get_vector_pos(), delayed_[i].caster_);
                break;
            default:
                break;
            }
            delayed_.erase(delayed_.begin() + i);
            i--;
            continue;
        }
    }

    for (int i = 0; i < static_cast<int>(channeled_.size()); ++i)
    {
        channeled_[i].time_remaining_ -= delta;
        channeled_[i].current_interval_ -= delta;
        if (channeled_[i].current_interval_ <= 0.0f)
        {
            if (channeled_[i].skill_.hide_character)
                channeled_[i].caster_->set_invisible(true);
            if (channeled_[i].skill_.sfx != "")
                sound::get()->play_sfx(channeled_[i].skill_.sfx );
            switch (channeled_[i].skill_.type)
            {
            case skill_type::instant:
                apply_subskill(channeled_[i].skill_, channeled_[i].target_->get_vector_pos(), channeled_[i].caster_);
                break;
            case skill_type::projectile:
                launch_projectile(channeled_[i].skill_,
                                  channeled_[i].caster_->get_world_pos().get_angle_to(channeled_[i].target_->get_world_pos()),
                                  channeled_[i].caster_);
                break;
            case skill_type::railgun:
                launch_projectile(channeled_[i].skill_,
                                  channeled_[i].caster_->get_world_pos().get_angle_to(channeled_[i].target_->get_world_pos()),
                                  channeled_[i].caster_,
                                  true,
                                  channeled_[i].target_);
                break;
            case skill_type::self:
                apply_subskill(channeled_[i].skill_, channeled_[i].caster_->get_vector_pos(), channeled_[i].caster_, true);
                break;
            case skill_type::teleport:
                teleport(channeled_[i].caster_, grid_->get_tile_at(channeled_[i].caster_->get_vector_pos()),
                        channeled_[i].target_);
                break;
            case skill_type::self_aoe:
                apply_subskill(channeled_[i].skill_, channeled_[i].caster_->get_vector_pos(), channeled_[i].caster_);
                break;
            default:
                break;
            }
            channeled_[i].interval_ *= channeled_[i].interval_reduction_;
            channeled_[i].current_interval_ = channeled_[i].interval_;
            if (channeled_[i].skill_.animation != "")
                channeled_[i].caster_->set_animation_state(channeled_[i].skill_.animation, true);
        }
        if (channeled_[i].time_remaining_ <= 0.0f)
        {
            channeled_.erase(channeled_.begin() + i);
            i--;
            continue;
        }
    }
}

bool skill_handler::add_cast(character_base* caster,
                             int index,
                             composite_skill* skill,
                             grid_tile* target)
{
    if (skill->is_teleport() &&
        !caster->is_ai() &&
        target->get_state() == tile_state::out_of_bounds)
        return false;
    if (caster->is_casting())
        interrupt(caster);
    if (caster->is_incapacitated())
        return false;
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
    return true;
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
    for (unsigned i = 0; i < channeled_.size(); ++i)
    {
        if (channeled_[i].caster_ == caster)
        {
            channeled_.erase(channeled_.begin() + i);
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
                               utils::yth_node* effect,
                               bool flipped)
{
    visual_effect temp(position, effect, false, 0.0, false);
    if (flipped)
        temp.orient_sprite(!flipped);
    effects_.push_back(temp);
}
void skill_handler::teleport(character_base* caster,
                             grid_tile* origin,
                             grid_tile* destination)
{
    unsigned radius = 1;
    while (destination->get_occupant() != NULL && radius < 10)
    {
        std::vector<grid_tile*> surrounding;
        grid_->get_tiles_in_radius(destination->get_vector_pos(), radius, surrounding);
        for (unsigned i = 0; i < surrounding.size(); ++i)
            if (surrounding[i]->get_occupant() == NULL)
                destination = surrounding[i];
        radius++;
    }
    origin->set_occupant(NULL);
    caster->update_pos(destination->get_world_pos(), destination->get_vector_pos());
    destination->set_occupant(caster);
}
}   // namespace encounter
}   // namespace icarus
