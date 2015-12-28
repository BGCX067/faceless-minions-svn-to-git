#include "icarus/encounter/character_base.hpp"
#include "icarus/utils.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/encounter/grid.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
character_base::character_base(grid* grid_ptr, const std::string& name)
:
    grid_ptr_(grid_ptr),
    target_position_(0, 0),
    prev_position_(0, 0),
    vector_position_(0, 0),
    target_vec_pos_(0, 0),
    move_speed_(0.0f),
    movement_step_(0.0f),
    is_moving_(false),
    health_(0),
    current_health_(0),
    name_(name),
    combat_text_(2.0f, 75.0f, 20, sf::Text::Style::Bold),
    is_casting_(false),
    cast_bar_(utils::rgb(0x435FBA), utils::rgb(0x3B3B3B), "0.0s"),
    path_timer_(0.0f),
    retry_target_(NULL),
    is_ai_(false),
    is_obstacle_(false),
    is_dead_(false),
    dmg_fade_timer_(0.15f),
    dmg_fade_current_(0.0f),
    dmg_fade_factor_(0.5f),
    minimum_damage_(0),
    maximum_damage_(0),
    dodge_chance_(0.0f),
    resist_chance_(0.0f),
    resist_amount_(0.0f),
    block_chance_(0.0f),
    block_amount_(0.0f),
    critical_chance_(0),
    critical_mod_(0.0f),
    speed_mod_(1.0f),
    phys_damage_mod_(1.0f),
    magic_damage_mod_(1.0f),
    damage_reduction_(0.0f),
    stored_damage_(0),
    flash_(false),
    flash_timer_(0.1f),
    flash_count_(0),
    render_(true),
    show_health_bar_(false),
    show_cast_bar_(false),
    incapacitated_(false),
    debug_(false),
    kills_(0),
    assists_(0),
    damage_dealt_(0),
    damage_received_(0),
    largest_taken_(0),
    largest_given_(0),
    friendly_fire_(0),
    hover_(false)
{
    debug_line_.setPrimitiveType(sf::PrimitiveType::Lines);
}

character_base::~character_base()
{

}

void character_base::set_target_path(std::vector<path_node*> new_path)
{
    for (unsigned i = 0; i < path_to_target_.size(); ++i)
        delete path_to_target_[i];
    path_to_target_.clear();
    path_to_target_ = new_path;
    for (unsigned i = 0; i < path_to_target_.size(); ++i)
        if (path_to_target_[i]->vec_pos == target_vec_pos_)
            path_to_target_.erase(path_to_target_.begin() + i,
                                  path_to_target_.end());
    retry_target_ = NULL;
}
void character_base::set_target_pos(const math::vector2f& new_pos)
{
    prev_position_ = get_world_pos();
    target_position_ = new_pos;
}
sf::FloatRect character_base::get_bounds(bool use_global) const
{
    if (use_global)
        return character_sprite_->getGlobalBounds();
    return character_sprite_->getLocalBounds();
}
void character_base::move_one_step()
{
    movement_step_ += (math::clamp(move_speed_ * speed_mod_, 0.0f, 1000.0f) * 0.01f *
                       state_manager::get()->get_delta());
    character_sprite_->setPosition(math::v2lerp(prev_position_,
                                                target_position_,
                                  math::clamp(movement_step_, 0.0f, 1.0f)).get_sf_vector());
    shadow_.set_position(math::vector2f(character_sprite_->getPosition()));
}
void character_base::pop_path()
{
    // TODO:: MEMORY LEAKS!
    path_to_target_.pop_back();
    movement_step_ = 0.0f;
}

void character_base::update_pos(const math::vector2f& sprite_pos_updated,
                                const math::vector2i& grid_pos_updated)
{
    character_sprite_->setPosition(sprite_pos_updated.get_sf_vector());
    vector_position_ = grid_pos_updated;
    target_position_ = sprite_pos_updated;
    target_vec_pos_ = vector_position_;
    this->set_animation_state("idle");
    shadow_.set_position(sprite_pos_updated);
}

void character_base::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!render_)
        return;
    target.draw(shadow_, states);
    const sf::Shader* default_shader = states.shader;
    if (flash_)
    {
        resource_handler::get()->get_shader("color_overlay")->setParameter("factor", 1.0);
        resource_handler::get()->get_shader("color_overlay")->setParameter("overlay_color", utils::rgb(0xFF0000));
        resource_handler::get()->get_shader("color_overlay")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("color_overlay");
    }
    else if (dmg_fade_current_ > 0.0f)
    {
        resource_handler::get()->get_shader("color_overlay")->setParameter("factor", (dmg_fade_current_ / dmg_fade_timer_) * dmg_fade_factor_);
        resource_handler::get()->get_shader("color_overlay")->setParameter("overlay_color", utils::rgb(0xFFFFFF));
        resource_handler::get()->get_shader("color_overlay")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("color_overlay");
    }
    else if (hover_)
    {
        resource_handler::get()->get_shader("outline")->setParameter("outline_color", utils::rgb(0xFF0000));
        resource_handler::get()->get_shader("outline")->setParameter("outline_width", 1.0);
        resource_handler::get()->get_shader("outline")->setParameter("alpha", 0.7);
        resource_handler::get()->get_shader("outline")->setParameter("img_width", character_sprite_->getTexture()->getSize().x);
        resource_handler::get()->get_shader("outline")->setParameter("img_height", character_sprite_->getTexture()->getSize().y);
        resource_handler::get()->get_shader("outline")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("outline");
    }
    target.draw(*character_sprite_, states);
    states.shader = default_shader;
    if (is_casting_ && show_cast_bar_)
        target.draw(cast_bar_, states);
    if (show_health_bar_)
        target.draw(hp_bar_, states);
    target.draw(combat_text_, states);
    if (debug_)
        target.draw(debug_line_, states);
}

void character_base::print_damage(int damage, damage_type::type dmg_type,
                                  int alt_damage)
{
    std::string text = "";
    sf::Color color(utils::rgb(0xffff00));
    unsigned text_size = 20;
    sf::Text::Style style = sf::Text::Regular;
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 1.0f);

    switch (dmg_type)
    {
    case damage_type::normal:
        text = utils::to_str(damage);
        break;
    case damage_type::critical:
        text = utils::to_str(damage) + " CRITICAL HIT!";
        color = utils::rgb(0xFF0000);
        text_size = 22;
        style = sf::Text::Bold;
        break;
    case damage_type::block:
        text = utils::to_str(damage) + " (" + utils::to_str(alt_damage)
               + " blocked!)";
        color = utils::rgb(0xE6E6E6);
        break;
    case damage_type::resist:
        text = utils::to_str(damage) + " (" + utils::to_str(alt_damage)
               + " resisted!)";
        color = utils::rgb(0x8CE6E6);
        break;
    case damage_type::dodge:
        text = "Dodge!";
        color = utils::rgb(0xFFFF00);
        break;
    case damage_type::critical_block:
        text = utils::to_str(damage) + " (" + utils::to_str(alt_damage)
               + " CRITICAL BLOCK!)";
        color = utils::rgb(0xE6E6E6);
        text_size = 22;
        style = sf::Text::Bold;
        break;
    case damage_type::critical_resist:
        text = utils::to_str(damage) + " (" + utils::to_str(alt_damage)
               + " CRITICAL RESIST!)";
        color = utils::rgb(0x8CE6E6);
        text_size = 22;
        style = sf::Text::Bold;
        break;
    case damage_type::critical_dodge:
        text = "CRITICAL DODGE!";
        color = utils::rgb(0xFFFF00);
        text_size = 22;
        style = sf::Text::Bold;
        break;
    case damage_type::overkill:
        text = utils::to_str(damage) + " (" + utils::to_str(alt_damage)
               + " OVERKILL!)";
        color = utils::rgb(0xE60000);
        text_size = 25;
        style = sf::Text::Bold;
        break;
    case damage_type::stun:
        text = "Stunned!";
        color = utils::rgb(0xE6E6E6);
        break;
    }
    combat_text_.add_text(text, color, pos, text_size, style);
}

void character_base::update_cast_bar(float remaining, float duration)
{
    cast_bar_.set_label(utils::to_str(math::to_precision(remaining, 1)) + "s");
    cast_bar_.set_bar_fill(math::clamp(1.0f - (remaining / duration), 0.0f, 1.0f));
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.9f);
    cast_bar_.set_position(pos);
}

bool character_base::apply_damage(int damage, uint8_t flags, character_base* source)
{
    if (is_dead_)
        return false;
    bool blocked = false;
    bool resisted = false;
    bool crit = flags & skill_flags::IS_CRITICAL;
    int damage_reduction = 0;
    damage -= damage * math::clamp(damage_reduction_, -1.0f, 1.0f);

    if ((flags & skill_flags::CAN_RESIST) && static_cast<unsigned>(rand()%100) < resist_chance_)
    {
        damage -= (damage_reduction = damage * resist_amount_);
        resisted = true;
    }
    else if ((flags & skill_flags::CAN_BLOCK) && static_cast<unsigned>(rand()%100) < block_chance_)
    {
        damage -= (damage_reduction = damage * block_amount_);
        blocked = true;
    }

    add_dmg_taken(damage);
    source->add_dmg_done(damage);
    if ((is_ai() && source->is_ai()) ||
        (!is_ai() && !source->is_ai()))
    {
        source->add_ff_dmg(damage);
    }
    if (dmg_contributors_.find(source) == dmg_contributors_.end())
        dmg_contributors_.insert(source);

    current_health_ -= damage;
    if (damage > 0)
        dmg_fade_current_ = dmg_fade_timer_;

    if (current_health_ < 0)
    {
        print_damage(damage, damage_type::overkill, abs(current_health_));
        death();
    }
    else if (resisted)
    {
        print_damage(damage, (crit ? damage_type::critical_resist : damage_type::resist), damage_reduction);
    }
    else if (blocked)
    {
        print_damage(damage, (crit ? damage_type::critical_block : damage_type::block), damage_reduction);
    }
    else
    {
        print_damage(damage, (crit ? damage_type::critical : damage_type::normal));
    }

    if (current_health_ == 0)
    {
        death();
    }
    if (is_dead_)
    {
        for(std::set<character_base*>::iterator it = dmg_contributors_.begin();
            it != dmg_contributors_.end(); ++it)
        {
            if (*it != source)
                (*it)->add_assist();
        }
        source->add_kill();
    }
    current_health_ = current_health_ < 0 ? 0 : current_health_;
    update_hp_bar(static_cast<float>(current_health_) / health_);
    return current_health_;
}

void character_base::apply_skill(const targeted_skill& skill)
{
    if (skill.type != skill_type::undefined && !is_dead_)
    {
        if ((skill.flags & skill_flags::CAN_DODGE) &&
            static_cast<unsigned>(rand()%100) < dodge_chance_)
        {
            bool crit = skill.flags & skill_flags::IS_CRITICAL;
            print_damage(0, (crit ? damage_type::critical_dodge : damage_type::dodge));

            for (unsigned i = 0; i < effects_.size(); ++i)
            {
                if (effects_[i].effect.type == effect_type::riposte)
                {
                    stored_damage_ = skill.damage;
                    remove_effect(effects_[i].effect);
                    effects_.erase(effects_.begin() + i);
                }
            }
        }
        else
        {
            if (skill.damage > 0)
                apply_damage(skill.damage, skill.flags, skill.source_);
            if (skill.effect.type != effect_type::undefined)
            {
                active_effect new_effect;
                new_effect.effect = skill.effect;
                new_effect.ticks_remaining = skill.effect.ticks;
                new_effect.time_left = skill.effect.duration;
                new_effect.tick_step = skill.effect.duration / skill.effect.ticks;
                new_effect.flags = skill.effect.flags;
                effects_.push_back(new_effect);
                apply_stat_mods(new_effect.effect);
            }
        }
    }
}

void character_base::apply_effect(const applied_effect& effect)
{
    if (is_dead_)
        return;
    if (effect.damage > 0)
        apply_damage(effect.damage, effect.flags, effect.source_);
}

void character_base::apply_stat_mods(const applied_effect& effect)
{
    if (effect.stun_)
    {
        incapacitated_++;
        print_damage(0, damage_type::stun);
    }
    speed_mod_ += effect.speed_mod;
    if (speed_mod_ <= 0.0f && is_moving_)
    {
        is_moving_ = false;
        set_animation_state("idle");
    }
    phys_damage_mod_ += effect.damage_mod;
    magic_damage_mod_ += effect.damage_mod;
    damage_reduction_ += effect.damage_reduction;
    dodge_chance_ += effect.dodge_mod;
}

void character_base::remove_effect(const applied_effect& effect)
{
    if (effect.stun_)
    {
        incapacitated_--;
        if (incapacitated_ < 0)
            incapacitated_ = 0;
    }

    speed_mod_ -= effect.speed_mod;
    if (speed_mod_ > 0.0f && !is_moving_ &&
        get_target_path().size() > 0)
    {
        is_moving_ = true;
        set_animation_state("move");
    }
    phys_damage_mod_ -= effect.damage_mod;
    magic_damage_mod_ -= effect.damage_mod;
    damage_reduction_ -= effect.damage_reduction;
    dodge_chance_ -= effect.dodge_mod;
}

void character_base::update_movement()
{
    if (dmg_fade_current_ > 0.0)
    {
        dmg_fade_current_ -= state_manager::get()->get_delta();
    }
    if (is_dead_)
    {
        if (flash_count_ < 5)
        {
            if (flash_timer_.update(state_manager::get()->get_delta()))
            {
                flash_ = !flash_;
                flash_timer_.reset();
                if (!flash_)
                    flash_count_++;
            }
        }
        else
        {
            render_ = false;
        }
        return;
    }
    else if (incapacitated_)
    {
        return;
    }
    if (this->get_world_pos() != this->get_target_pos())
    {
        if (!is_moving_)
        {
            is_moving_ = true;
        }
        this->set_animation_state("move");
        this->move_one_step();
        if (this->get_step() >= 0.5f)
        {
            if (this->get_target_vec_pos() != this->get_vector_pos())
            {
                grid_ptr_->get_tile_at(this->get_target_vec_pos())->set_occupant(this);
                grid_ptr_->get_tile_at(this->get_target_vec_pos())->unreserve();
                grid_ptr_->get_tile_at(this->get_vector_pos())->set_occupant(NULL);
                this->get_vector_pos() = this->get_target_vec_pos();
            }
        }
    }
    else
    {
        if (this->get_target_path().size() == 0)
        {
            if (retry_target_ != NULL)
            {
                if (path_timer_ > 0.0f)
                {
                    path_timer_ -= state_manager::get()->get_delta();
                }
                else if (!grid_ptr_->get_path_to_tile(retry_target_->get_vector_pos(), this))
                {
                    path_timer_ = 0.5f;
                }
            }
            if (is_moving_)
            {
                is_moving_ = false;
            }
            this->set_animation_state("idle");
            return;
        }
        grid_tile* target_tile = grid_ptr_->get_tile_at(this->get_target_path().back()->
                                                       vec_pos);
        if (target_tile->get_state() != tile_state::unoccupied)
        {
            if (grid_ptr_->get_path_to_tile(path_to_target_[0]->vec_pos, this))
            {
                target_tile = grid_ptr_->get_tile_at(this->get_target_path().back()->
                                                       vec_pos);
            }
            else
            {
                retry_target_ = grid_ptr_->get_tile_at(path_to_target_[0]->vec_pos);
                path_timer_ = 0.5f;
                for (unsigned i = 0; i < path_to_target_.size(); ++i)
                    delete path_to_target_[i];
                path_to_target_.clear();
                return;
            }
        }
        if (this->get_target_pos() != target_tile->get_world_pos())
        {
            this->set_target_pos(target_tile->get_world_pos());
            this->set_target_vec_pos(target_tile->get_vector_pos());

            if(this->get_target_pos().x >= this->get_world_pos().x)
                this->orient_sprite(true);
            else
                this->orient_sprite(false);
        }
        this->pop_path();
        target_tile->reserve_tile();
    }
}

void character_base::death()
{
    is_dead_ = true;
    incapacitated_ = false;
    clear_path();
    grid_ptr_->get_tile_at(vector_position_)->set_occupant(NULL);
    flash_ = true;
    flash_timer_.reset();
    flash_count_ = 0;
}

void character_base::clear_path()
{
    for (unsigned i = 0; i < path_to_target_.size(); ++i)
        delete path_to_target_[i];
    path_to_target_.clear();
    retry_target_ = NULL;
}

void character_base::print_dodge(bool critical)
{
    print_damage(0, (critical ? damage_type::critical_dodge : damage_type::dodge));
}
void character_base::update_hp_bar(float fraction)
{
    hp_bar_.set_bar_fill(fraction);
}
}   // namespace encounter
}   // namespace icarus
