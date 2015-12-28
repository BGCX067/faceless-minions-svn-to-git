#include "icarus/encounter/ui/icon.hpp"
#include "icarus/state_manager.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
icon::icon(const sf::Texture* texture)
:
    icon_(*texture),
    outline_(35.0f, 6),
    autocast_indicator_(math::vector2f(0.0f, 0.0f),
                        resource_handler::get()->get_root_node("effects")->child("interface")->child("autocast"),
                        true, 0.0f),
    cooldown_complete_(math::vector2f(0.0f, 0.0f),
                        resource_handler::get()->get_root_node("effects")->child("interface")->child("cooldown_complete"),
                        false, 0.0f),
    color_factor_(1.0f),
    outline_visible_(false),
    autocast_enabled_(false),
    spell_ready_(true),
    cooldown_timer_(0.0f),
    disabled_(false)
{
    outline_.rotate(90.0f);
    outline_.setOrigin(outline_.getLocalBounds().width/2.0f,
                       outline_.getLocalBounds().height/2.0f);
    outline_.setFillColor(utils::argb(0x00000000));
    outline_.setOutlineColor(utils::argb(0xCCEEEE00));
    outline_.setOutlineThickness(5.0f);

    tooltip_type_ = icarus::interface::tooltip_type::generic;
    base_tooltip::enabled_ = true;

    hover_bounds_ = icon_.getGlobalBounds();
    cooldown_complete_.stop();
}
icon::~icon()
{
}

void icon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const sf::Shader* default_shader = states.shader;
    resource_handler::get()->get_shader("grayscale_luminosity")->setParameter("factor", (disabled_ ? 0.0f : color_factor_));
    resource_handler::get()->get_shader("grayscale_luminosity")->setParameter("texture", *icon_.getTexture());
    states.shader = resource_handler::get()->get_shader("grayscale_luminosity");
    target.draw(icon_, states);
    states.shader = default_shader;
    if (cooldown_complete_.is_active() && !disabled_)
        target.draw(cooldown_complete_, states);
    if (autocast_enabled_ && !disabled_)
        target.draw(autocast_indicator_, states);
    if (outline_visible_ && !disabled_)
        target.draw(outline_, states);
}
void icon::set_texture(const sf::Texture* new_texture)
{
    icon_.setTexture(*new_texture);
    hover_bounds_ = icon_.getGlobalBounds();
}
void icon::set_position(const sf::Vector2f pos)
{
    icon_.setPosition(pos);
    autocast_indicator_.set_world_pos(math::vector2f(pos) + math::vector2f(5.0f, 5.0f));
    cooldown_complete_.set_world_pos(math::vector2f(pos) + math::vector2f(5.0f, 5.0f));
    sf::Vector2f o_pos = pos;
    o_pos.x += icon_.getLocalBounds().width/2.0f;
    o_pos.y += icon_.getLocalBounds().height/2.0 - 5.0f;
    outline_.setPosition(o_pos);
    hover_bounds_ = icon_.getGlobalBounds();
}
void icon::set_cooldown(float duration)
{
    cooldown_timer_.set(duration);
    spell_ready_ = false;
}
void icon::set_cooldown(float duration, float current)
{
    cooldown_timer_.set(duration, current);
    spell_ready_ = false;
}
void icon::stop_cooldown()
{
    cooldown_timer_.stop();
    spell_ready_ = true;
}
void icon::update()
{
    if (disabled_)
        return;
    double delta = state_manager::get()->get_delta();
    if ((cooldown_timer_.update(delta)) && !spell_ready_)
    {
        cooldown_complete_.reset();
        spell_ready_ = true;
    }
    color_factor_ = 1.0f - cooldown_timer_.get_factor();
    if (autocast_enabled_)
    {
        autocast_indicator_.update(delta);
    }
    if (cooldown_complete_.is_active())
    {
        cooldown_complete_.update(delta);
    }
}

void icon::update_text(utils::yth_node* skill_node)
{
    // TODO::fix
}
} // namespace ui
} // namespace encounter
} // namespace icarus
