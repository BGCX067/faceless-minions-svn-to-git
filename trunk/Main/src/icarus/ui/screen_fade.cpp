#include "icarus/ui/screen_fade.hpp"
#include "icarus/state_manager.hpp"

namespace icarus
{
namespace ui
{
screen_fade::screen_fade(const utils::rgb& color, float duration, bool start_opaque)
:
    timer_(duration),
    duration_(duration),
    fade_in_(!start_opaque),
    active_(false)
{
    screen_.setSize(state_manager::get()->get_dimensions());

    screen_.setFillColor(color);
    if (!fade_in_)
    {
        sf::Color new_color(color);
        new_color.a = 0;
        screen_.setFillColor(new_color);
    }
}
screen_fade::~screen_fade()
{
}

void screen_fade::reset(bool fade_in)
{
    fade_in_ = fade_in;
    sf::Color new_color = screen_.getFillColor();
    new_color.a = fade_in ? static_cast<uint8_t>(0) : static_cast<uint8_t>(255);
    screen_.setFillColor(new_color);
    active_ = true;

    timer_ = duration_;
}
bool screen_fade::update(double delta)
{
    if (!active_)
        return false;
    timer_ -= delta;
    sf::Color new_color = screen_.getFillColor();
    uint8_t amount = static_cast<uint8_t>(255.0 * (timer_ / duration_));
    if (fade_in_)
        amount = uint8_t(255) - amount;
    new_color.a = math::clamp(amount, static_cast<uint8_t>(0), static_cast<uint8_t>(255));
    screen_.setFillColor(new_color);
    if (timer_ <= 0.0f)
        active_ = false;
    return (timer_ <= 0.0f);
}
void screen_fade::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(screen_, states);
}
}
}
