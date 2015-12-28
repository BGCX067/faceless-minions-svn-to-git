#include "icarus/encounter/skill_handler.hpp"

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
    // cast the damn skill
    std::cout << cast.skill_->get_name() << " was cast!" << std::endl;
    std::vector<targeted_skill> subskills = cast.skill_->get_subskills();
    for (unsigned i = 0; i < subskills.size(); ++i)
    {
        switch (subskills[i].type)
        {
        case skill_type::instant:
            apply_subskill(subskills[i], cast.target_->get_vector_pos());
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

void skill_handler::apply_subskill(const targeted_skill& subskill,
                                   const math::vector2i& vec_pos)
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
            occupant->apply_skill(subskill);
        }
    }
}

void skill_handler::launch_projectile(const targeted_skill& subskill,
                                      float angle,
                                      character_base* caster)
{
    float h_ratio = sin(angle);
    float v_ratio = cos(angle);
    projectile new_projectile(700.0f);
    new_projectile.set_skill(subskill);
    new_projectile.set_speed_ratios(h_ratio, v_ratio);
    new_projectile.update_pos(caster->get_world_pos(), caster->get_vector_pos());
    new_projectile.set_caster(caster);
    projectiles_.push_back(new_projectile);
}

void skill_handler::update()
{
    for (unsigned i = 0; i < active_casts_.size(); ++i)
    {
        if (active_casts_[i].time_remaining_ <= 0.0f)
        {
            cast_skill(active_casts_[i]);
            active_casts_.erase(active_casts_.begin() + i);
        }
        else
        {
            active_casts_[i].time_remaining_ -= state_manager::get_instance().get_delta();
            active_casts_[i].caster_->update_cast_bar(active_casts_[i].time_remaining_,
                                                      active_casts_[i].skill_->get_cast_time());
        }
    }
    for (int i = 0; i < static_cast<int>(projectiles_.size()); ++i)
    {
        projectiles_[i].update(state_manager::get_instance().get_delta());
        grid_tile* current_tile = grid_->get_tile_at(projectiles_[i].get_world_pos());
        if (current_tile)
        {
            if (current_tile->get_occupant() &&
                current_tile->get_occupant() != projectiles_[i].get_caster())
            {
                apply_subskill(projectiles_[i].get_subskill(),
                               current_tile->get_vector_pos());
                projectiles_.erase(projectiles_.begin() + i);
                i--;
                continue;
            }
        }
        else
        {
            projectiles_.erase(projectiles_.begin() + i);
            i--;
            continue;
        }
    }
}

void skill_handler::add_cast(character_hero* caster,
                             composite_skill* skill,
                             grid_tile* target)
{
    if (caster->is_casting())
        interrupt(caster);
    skill_cast new_cast(skill, caster, target);
    active_casts_.push_back(new_cast);
    caster->set_casting(true);
    std::cout << "casting " << skill->get_name() << "!" << std::endl;
}

void skill_handler::interrupt(character_hero* caster)
{
    for (unsigned i = 0; i < active_casts_.size(); ++i)
    {
        if (active_casts_[i].caster_ == caster)
        {
            std::cout << "the casting of " << active_casts_[i].skill_->get_name() << " was interrupted!" << std::endl;
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
}
}   // namespace encounter
}   // namespace icarus
