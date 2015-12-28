#include "icarus/resource_handler.hpp"
#include "icarus/encounter/combat_text.hpp"

namespace icarus
{
namespace encounter
{
combat_text::combat_text(float default_duration, float default_speed,
                         unsigned default_size, sf::Text::Style default_style)
:
    default_duration_(default_duration),
    default_speed_(default_speed),
    gravity_(125.0f),
    default_size_(default_size),
    default_style_(default_style),
    fade_by_default_(true)
{
}
combat_text::~combat_text()
{
    for (unsigned i = 0; i < text_list_.size(); ++i)
    {
        delete text_list_[i].text_;
    }
}

void combat_text::update(double delta)
{
    for (int i = 0; i < static_cast<int>(text_list_.size()); ++i)
    {
        text_list_[i].time_left_ -= delta;
        if (text_list_[i].time_left_ <= 0.0f)
        {
            delete text_list_[i].text_;
            text_list_.erase(text_list_.begin() + i);
            i--;
            continue;
        }
        text_list_[i].move_vector_ += math::vector2f(0.0f, gravity_) * delta;
        text_list_[i].text_->move((text_list_[i].move_vector_ * delta));
        text_list_[i].reduce_alpha(delta);
    }
}

void combat_text::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (unsigned i = 0; i < text_list_.size(); ++i)
    {
        target.draw(*(text_list_[i].text_), states);
    }
}

void combat_text::add_text(const std::string& text,
                           const sf::Color& color,
                           const math::vector2f& position,
                           unsigned text_size,
                           sf::Text::Style style,
                           float duration,
                           float speed)
{
    combat_text::text new_text;
    new_text.text_ = new sf::Text();
    new_text.text_->setString(text);
    new_text.text_->setFont(*resource_handler::get()->get_font("numbers"));
    new_text.text_->setColor(color);
    new_text.text_->setStyle(style);
    new_text.text_->setCharacterSize(text_size);
    new_text.time_left_ = duration;

    float angle = -((rand()%90) + 135.0f);
    math::vector2f move_vec(sin(math::deg_to_rad(angle)), cos(math::deg_to_rad(angle)));
    new_text.move_vector_ = move_vec.normalized() * speed;
    new_text.fade_total_ = 255.0f;
    new_text.fade_amount_ = 255 / duration;
    new_text.text_->setOrigin(new_text.text_->getLocalBounds().width * 0.5f,
                              new_text.text_->getLocalBounds().height * 0.5f);
    new_text.text_->setPosition(position.get_sf_vector());
    text_list_.push_back(new_text);
}
}   // namespace encounter
}   // namespace icarus
