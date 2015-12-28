#include "icarus/encounter/ui_hp_bar.hpp"

namespace icarus
{
namespace encounter
{
ui_hp_bar::ui_hp_bar()
:
    ui_bar(sf::Color::Green, sf::Color::Black, "hp"),
    damage_bar_(NULL),
    reduction_speed_(20.0f),
    reduction_delay_(0.5f)
{
    damage_bar_ = new sf::RectangleShape(sf::Vector2f(bar_width_, bar_height_));
    damage_bar_->setFillColor(sf::Color::White);
    damage_bar_->setOrigin(0.0f, bar_height_ * 0.5f);
}
ui_hp_bar::~ui_hp_bar()
{
    if (damage_bar_)
        delete damage_bar_;
}

void ui_hp_bar::set_color(const sf::Color& main_color)
{
    bar_->setFillColor(main_color);
}

void ui_hp_bar::set_position(const math::vector2f& new_pos)
{
    background_->setPosition(new_pos.get_sf_vector());
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
    damage_bar_->setPosition(bar_->getPosition());
    set_label_position(label_v_pos_, label_h_pos_);
}

void ui_hp_bar::update_bar(float delta)
{
    if (current_delay_ <= 0.0f && damage_bar_->getSize().x > bar_->getSize().x)
    {
        float new_width = damage_bar_->getSize().x - (reduction_speed_ * delta);
        damage_bar_->setSize(sf::Vector2f(new_width, damage_bar_->getSize().y));
    }
    else if (current_delay_ > 0.0f)
    {
        current_delay_ -= delta;
    }
}

void ui_hp_bar::set_bar_fill(float percentage)
{
    math::clamp(percentage, 0.0f, 1.0f);
    bar_->setSize(sf::Vector2f(percentage * bar_width_, bar_height_));
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
    current_delay_ = reduction_delay_;
}

void ui_hp_bar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*background_, states);
    target.draw(*damage_bar_, states);
    target.draw(*bar_, states);
    target.draw(*label_, states);
}
} // namespace encounter
} // namespace icarus
