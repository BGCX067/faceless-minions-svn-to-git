#ifndef ICARUS_OVERWORLD_GUI_ICON_HPP_
#define ICARUS_OVERWORLD_GUI_ICON_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/ui/base_tooltip.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class icon : public sf::Drawable, public icarus::ui::base_tooltip
{
private:
    sf::Sprite sprite_;
    bool active_;
    bool highlight_;
    bool visible_;
public:
    icon(bool is_item = false);
    virtual ~icon();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_texture(const sf::Texture& tex);
    bool contains(const math::vector2f& pos) const { return sprite_.getGlobalBounds().contains(pos); }
    void set_active(bool state) { active_ = state; }
    void set_highlight(bool state) { highlight_ = state; }
    void set_position(const math::vector2f& pos) { sprite_.setPosition(pos); hover_bounds_ = sprite_.getGlobalBounds(); }
    math::vector2f get_position() const { return math::vector2f(sprite_.getPosition()); }
    math::vector2f get_dimensions() const { return math::vector2f(sprite_.getLocalBounds().width, sprite_.getLocalBounds().height); }
    bool is_visible() const { return visible_; }
    void set_visible(bool state) { visible_ = state; }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_ICON_HPP_
