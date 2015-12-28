#include "icarus/encounter/ui_bar.hpp"

namespace icarus
{
namespace encounter
{
ui_bar::ui_bar(const sf::Color& bar_color,
               const sf::Color& background_color,
               const std::string& label)
:
    background_(NULL),
    bar_(NULL),
    label_(NULL),
    label_v_pos_(label_position::v_center),
    label_h_pos_(label_position::h_center),
    bar_width_(125.0f),
    bar_height_(15.0f)
{
    background_ = new sf::RectangleShape(sf::Vector2f(bar_width_, bar_height_));
    background_->setOutlineThickness(1.0f);
    background_->setFillColor(background_color);
    background_->setOutlineColor(background_color);
    background_->setOrigin(background_->getSize().x * 0.5f,
                           background_->getSize().y * 0.5f);

    bar_ = new sf::RectangleShape(sf::Vector2f(bar_width_, bar_height_));
    bar_->setFillColor(bar_color);
    bar_->setOrigin(0.0f, bar_->getSize().y * 0.5f);

    label_ = new sf::Text(label);
    label_->setCharacterSize(static_cast<int>(12));
    label_->setColor(sf::Color::White);
    label_->setStyle(sf::Text::Style::Bold);
}

ui_bar::~ui_bar()
{
    if (background_)
        delete background_;
    if (bar_)
        delete bar_;
    if (label_)
        delete label_;
}

void ui_bar::set_label(const std::string& new_label)
{
    label_->setString(new_label);
    math::vector2i new_origin;
    switch (label_v_pos_)
    {
    case label_position::v_top:
        new_origin.y = label_->getLocalBounds().height;
        break;
    case label_position::v_bottom:
        new_origin.y = 0;
        break;
    case label_position::v_center:
        new_origin.y = label_->getLocalBounds().height * 0.5f;
        break;
    }
    switch (label_h_pos_)
    {
    case label_position::h_left:
        new_origin.x = label_->getLocalBounds().width;
        break;
    case label_position::h_right:
        new_origin.x = 0;
        break;
    case label_position::h_center:
        new_origin.x = label_->getLocalBounds().width * 0.5f;
        break;
    }
    label_->setOrigin(new_origin.x,
                      new_origin.y);
}

void ui_bar::set_bar_fill(float percentage)
{
    math::clamp(percentage, 0.0f, 1.0f);
    bar_->setSize(sf::Vector2f(percentage * bar_width_, bar_height_));
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
}

void ui_bar::set_label_position(label_position::vertical v_pos,
                        label_position::horizontal h_pos)
{
    label_h_pos_ = h_pos;
    label_v_pos_ = v_pos;
    math::vector2f new_pos;
    switch (v_pos)
    {
    case label_position::v_top:
        new_pos.y = -bar_height_;
        break;
    case label_position::v_bottom:
        new_pos.y = bar_height_ * 0.5f;
        break;
    case label_position::v_center:
        new_pos.y = -3.0f;
        break;
    }
    switch (h_pos)
    {
    case label_position::h_left:
        new_pos.x = -(bar_width_ * 0.5f + 3.0f);
        break;
    case label_position::h_right:
        new_pos.x = bar_width_ * 0.5f + 3.0f;
        break;
    case label_position::h_center:
        new_pos.x = 0.0f;
        break;
    }
    new_pos = math::vector2f(background_->getPosition()) + new_pos;
    label_->setPosition(static_cast<int>(new_pos.x),
                        static_cast<int>(new_pos.y));
}

void ui_bar::set_position(const math::vector2f& new_pos)
{
    background_->setPosition(new_pos.get_sf_vector());
    bar_->setPosition(background_->getPosition().x -
                      background_->getSize().x * 0.5f,
                      background_->getPosition().y);
    set_label_position(label_v_pos_, label_h_pos_);
}

math::vector2f ui_bar::get_position()
{
    return math::vector2f(background_->getPosition());
}

void ui_bar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*background_, states);
    target.draw(*bar_, states);
    target.draw(*label_, states);
}
} // namespace encounter
} // namespace icarus
