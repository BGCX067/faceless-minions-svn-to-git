#ifndef ICARUS_ENCOUNTER_UI_ICON_HPP_
#define ICARUS_ENCOUNTER_UI_ICON_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/utils.hpp"
#include "icarus/interface/base_tooltip.hpp"
#include "icarus/encounter/visual_effect.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
class icon : public sf::Drawable, public icarus::interface::base_tooltip
{
private:
    sf::Sprite icon_;
    sf::CircleShape outline_;
    visual_effect autocast_indicator_;
    visual_effect cooldown_complete_;
    float color_factor_;
    bool outline_visible_;
    bool autocast_enabled_;
    bool spell_ready_;
    utils::countdown cooldown_timer_;
    bool disabled_;

public:
    icon(const sf::Texture* texture);
    virtual ~icon();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_texture(const sf::Texture* new_texture);
    void set_position(const sf::Vector2f pos);
    void set_cooldown(float duration);
    void set_cooldown(float duration, float current);
    void stop_cooldown();
    void update();
    void update_text(utils::yth_node* skill_node);

    void set_color_factor_(float factor) { color_factor_ = factor; }
    float set_color_factor_() const { return color_factor_; }
    void set_outline_visible(bool visible) { outline_visible_ = visible; }
    bool get_outline_visible() const { return outline_visible_; }
    void set_autocast_enabled(bool state) { autocast_enabled_ = state; }
    bool get_autocast_state() const { return autocast_enabled_; }
    sf::FloatRect get_bounds() const { return icon_.getGlobalBounds(); }
    math::vector2f get_position() const { return math::vector2f(icon_.getPosition()); }
    void set_position(float x, float y) { set_position(sf::Vector2f(x, y)); }
    bool is_disabled() const { return disabled_; }
    void set_disabled(bool status) { disabled_ = status; }
};
} // namespace ui
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_UI_ICON_HPP_
