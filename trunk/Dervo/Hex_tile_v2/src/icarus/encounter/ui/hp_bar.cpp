#include "icarus/encounter/ui/hp_bar.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
hp_bar::hp_bar()
:
    bar(utils::rgb(0x43BA47), utils::rgb(0x3B3B3B), "Health"),
    damage_bar_(NULL),
    reduction_speed_(20.0f),
    reduction_delay_(0.5f)
{
    damage_bar_ = new sf::RectangleShape(sf::Vector2f(bar_width_, bar_height_));
    damage_bar_->setFillColor(utils::rgb(0xFFFFFF));
    damage_bar_->setOrigin(0.0f, bar_height_ * 0.5f);
}
hp_bar::~hp_bar()
{
    if (damage_bar_)
        delete damage_bar_;
}

void hp_bar::set_color(const sf::Color& main_color)
{
    bar_->setFillColor(main_color);
}

void hp_bar::set_position(const math::vector2f& new_pos)
{
    background_->setPosition(new_pos.get_sf_vector());
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
    damage_bar_->setPosition(bar_->getPosition());
    set_label_position(label_v_pos_, label_h_pos_);
}

void hp_bar::set_size(const math::vector2f& new_size)
{
    bar_width_ = new_size.x;
    bar_height_ = new_size.y;
    background_->setSize(new_size);
    background_->setOrigin(background_->getSize().x * 0.5f,
                           background_->getSize().y * 0.5f);

    damage_bar_->setSize(new_size);
    damage_bar_->setOrigin(0.0f, damage_bar_->getSize().y * 0.5f);
    bar_->setSize(new_size);
    bar_->setOrigin(0.0f, bar_->getSize().y * 0.5f);
}

void hp_bar::update_bar(float delta)
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

void hp_bar::set_bar_fill(float percentage)
{
    math::clamp(percentage, 0.0f, 1.0f);
    bar_->setSize(sf::Vector2f(percentage * bar_width_, bar_height_));
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
    current_delay_ = reduction_delay_;
}

void hp_bar::set_fill_instant(float percentage)
{
    math::clamp(percentage, 0.0f, 1.0f);
    bar_->setSize(sf::Vector2f(percentage * bar_width_, bar_height_));
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
    damage_bar_->setSize(sf::Vector2f(bar_->getSize().x, bar_->getSize().y));
}

void hp_bar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*background_, states);
    target.draw(*damage_bar_, states);
    target.draw(*bar_, states);
    target.draw(*label_, states);
}
} // namespace ui
} // namespace encounter
} // namespace icarus
