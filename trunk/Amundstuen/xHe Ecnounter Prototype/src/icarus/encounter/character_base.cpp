#include "icarus/encounter/character_base.hpp"
#include "icarus/utilities.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/encounter/grid.hpp"

namespace icarus
{
namespace encounter
{
character_base::character_base(grid* grid_ptr, std::string name, std::string directory):
    animate(directory),
    grid_ptr_(grid_ptr),
    target_position_(0, 0),
    prev_position_(0, 0),
    vector_position_(0, 0),
    target_vec_pos_(0, 0),
    move_speed_(0.0f),
    movement_step_(0.0f),
    name_(name),
    combat_text_(4.0f, 30.0f, 20, sf::Text::Style::Bold),
    is_casting_(false),
    cast_bar_(sf::Color::Blue, sf::Color::Black, "0.0s")
{

}

character_base::character_base(grid* grid_ptr, std::string name)
:
    grid_ptr_(grid_ptr),
    target_position_(0, 0),
    prev_position_(0, 0),
    vector_position_(0, 0),
    target_vec_pos_(0, 0),
    move_speed_(0.0f),
    movement_step_(0.0f),
    name_(name),
    combat_text_(4.0f, 30.0f, 20, sf::Text::Style::Bold),
    is_casting_(false),
    cast_bar_(sf::Color::Blue, sf::Color::Black, "0.0s")
{
    std::cout << "No directory given for " << name << std::endl;
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
    character_sprite_->setPosition(math::v2lerp(prev_position_,
                                                target_position_,
                                  math::clamp(movement_step_, 0.0f, 1.0f)).get_sf_vector());
    movement_step_ += (math::clamp(move_speed_, 0.0f, 1000.0f) * 0.01f *
                       state_manager::get_instance().get_delta());
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
    target.draw(*character_sprite_, states);
    if (is_casting_)
        target.draw(cast_bar_, states);
    target.draw(hp_bar_, states);
    target.draw(combat_text_, states);
}

void character_base::print_damage(int damage, damage_type::type dmg_type,
                                  int alt_damage)
{
    std::string text = "";
    sf::Color color(sf::Color::White);
    unsigned text_size = 20;
    sf::Text::Style style = sf::Text::Regular;
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.5f);

    switch (dmg_type)
    {
    case damage_type::normal:
        text = utilities::to_str(damage);
        break;
    case damage_type::critical:
        text = utilities::to_str(damage);
        color = sf::Color::Red;
        text_size = 22;
        style = sf::Text::Bold;
        break;
    case damage_type::block:
        text = utilities::to_str(damage) + " (" + utilities::to_str(alt_damage)
               + " blocked)";
        color = sf::Color(230, 230, 230, 255);
        break;
    case damage_type::resist:
        text = utilities::to_str(damage) + " (" + utilities::to_str(alt_damage)
               + " resisted)";
        color = sf::Color(140, 230, 230, 255);
        break;
    case damage_type::overkill:
        text = utilities::to_str(damage) + " (" + utilities::to_str(alt_damage)
               + " overkill)";
        color = sf::Color(230, 0, 0, 255);
        text_size = 25;
        style = sf::Text::Bold;
        break;
    }
    combat_text_.add_text(text, color, pos, text_size, style);
}

void character_base::update_cast_bar(float remaining, float duration)
{
    cast_bar_.set_label(utilities::to_str(math::to_precision(remaining, 1)) + "s");
    cast_bar_.set_bar_fill(math::clamp(1.0f - (remaining / duration), 0.0f, 1.0f));
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.4f);
    cast_bar_.set_position(pos);
}

bool character_base::apply_damage(int damage)
{
    current_health_ -= damage;

    if (current_health_ < 0)
    {
        print_damage(damage, damage_type::overkill, abs(current_health_));
        current_health_ = 0;
    }
    else
    {
        print_damage(damage, damage_type::normal);
    }
    hp_bar_.set_bar_fill(static_cast<float>(current_health_) / health_);

    return current_health_;
}

void character_base::apply_skill(const targeted_skill& skill)
{
    std::cout << name_ << " took " << skill.damage << " damage from " << skill.name << "!" << std::endl;
    if (skill.effect.type != effect_type::undefined)
    {
        active_effect new_effect;
        new_effect.effect = skill.effect;
        new_effect.ticks_remaining = skill.effect.ticks;
        new_effect.time_left = skill.effect.duration;
        new_effect.tick_step = skill.effect.duration / skill.effect.ticks;
        effects_.push_back(new_effect);

        apply_damage(skill.damage);
    }
}

void character_base::apply_effect(const applied_effect& effect)
{
    apply_damage(effect.damage);
    std::cout << name_ << " took " << effect.damage << " damage from " << effect.name << "!" << std::endl;
}

void character_base::update_movement()
{
    if (this->get_world_pos() != this->get_target_pos())
    {
        this->move_one_step();
        if (this->get_step() >= 0.5f)
        {
            if (this->get_target_vec_pos() != this->get_vector_pos())
            {
                grid_ptr_->get_tile_at(this->get_target_vec_pos())->set_occupant(this);
                grid_ptr_->get_tile_at(this->get_vector_pos())->set_occupant(NULL);
                this->get_vector_pos() = this->get_target_vec_pos();
            }
        }
    }
    else
    {
        if (this->get_target_path().size() == 0)
        {
            this->orient_sprite(true);
            return;
        }
        grid_tile* target_tile = grid_ptr_->get_tile_at(this->get_target_path().back()->
                                                       vec_pos);
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
    }
}
}   // namespace encounter
}   // namespace icarus
