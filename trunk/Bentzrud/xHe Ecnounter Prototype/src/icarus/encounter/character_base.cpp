#include "icarus/encounter/character_base.hpp"
#include "icarus/utils.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/encounter/grid.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
character_base::character_base(grid* grid_ptr, std::string name)
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
    combat_text_(4.0f, 30.0f, 20, sf::Text::Style::Bold),
    is_casting_(false),
    cast_bar_(utils::rgb(0x435FBA), utils::rgb(0x3B3B3B), "0.0s"),
    path_timer_(0.0f),
    retry_target_(NULL),
    is_ai_(false),
    is_dead_(false),
    dodge_chance_(0.0f),
    resist_chance_(0.0f),
    resist_amount_(0.0f),
    block_chance_(0.0f),
    block_amount_(0.0f),
    critical_chance_(0),
    critical_mod_(0.0f),
    speed_mod_(1.0f),
    phys_damage_mod_(1.0),
    magic_damage_mod_(1.0),
    flash_(false),
    flash_timer_(0.1f),
    flash_count_(0),
    render_(true),
    show_health_bar_(false),
    show_cast_bar_(false)
{

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
    movement_step_ += (math::clamp(move_speed_, 0.0f, 1000.0f) * 0.01f *
                       state_manager::get()->get_delta());
    character_sprite_->setPosition(math::v2lerp(prev_position_,
                                                target_position_,
                                  math::clamp(movement_step_, 0.0f, 1.0f)).get_sf_vector());
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
}

void character_base::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!render_)
        return;
    const sf::Shader* default_shader = states.shader;
    if (flash_)
    {
        resource_handler::get()->get_shader("color_overlay")->setParameter("overlay_color", utils::rgb(0xFFFFFF));
        resource_handler::get()->get_shader("color_overlay")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("color_overlay");
    }
    target.draw(*character_sprite_, states);
    states.shader = default_shader;
    if (is_casting_ && show_cast_bar_)
        target.draw(cast_bar_, states);
    if (show_health_bar_)
        target.draw(hp_bar_, states);
    target.draw(combat_text_, states);
}

void character_base::print_damage(int damage, damage_type::type dmg_type,
                                  int alt_damage)
{
    std::string text = "";
    sf::Color color(utils::rgb(0xFFFFFF));
    unsigned text_size = 20;
    sf::Text::Style style = sf::Text::Regular;
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.5f);

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
    }
    combat_text_.add_text(text, color, pos, text_size, style);
}

void character_base::update_cast_bar(float remaining, float duration)
{
    cast_bar_.set_label(utils::to_str(math::to_precision(remaining, 1)) + "s");
    cast_bar_.set_bar_fill(math::clamp(1.0f - (remaining / duration), 0.0f, 1.0f));
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.4f);
    cast_bar_.set_position(pos);
}

bool character_base::apply_damage(int damage, uint8_t flags)
{
    bool blocked = false;
    bool resisted = false;
    bool crit = flags & skill_flags::IS_CRITICAL;
    int damage_reduction = 0;

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

    current_health_ -= damage;

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
        }
        else
        {
            apply_damage(skill.damage, skill.flags);
            if (skill.effect.type != effect_type::undefined)
            {
                active_effect new_effect;
                new_effect.effect = skill.effect;
                new_effect.ticks_remaining = skill.effect.ticks;
                new_effect.time_left = skill.effect.duration;
                new_effect.tick_step = skill.effect.duration / skill.effect.ticks;
                new_effect.flags = skill.effect.flags;
                effects_.push_back(new_effect);
            }
        }
    }
}

void character_base::apply_effect(const applied_effect& effect)
{
    if (is_dead_)
        return;
    apply_damage(effect.damage, effect.flags);
}

void character_base::update_movement()
{
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
    if (this->get_world_pos() != this->get_target_pos())
    {
        is_moving_ = true;
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
        is_moving_ = false;
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
