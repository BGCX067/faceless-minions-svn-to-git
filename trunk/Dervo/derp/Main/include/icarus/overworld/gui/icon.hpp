#ifndef ICARUS_OVERWORLD_GUI_ICON_HPP_
#define ICARUS_OVERWORLD_GUI_ICON_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/interface/base_tooltip.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class icon : public sf::Drawable, public icarus::interface::base_tooltip
{
private:
    sf::Sprite sprite_;
    bool active_;
    bool highlight_;
public:
    icon();
    virtual ~icon();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_texture(const sf::Texture& tex);
    bool contains(const math::vector2f& pos) const { return sprite_.getGlobalBounds().contains(pos); }
    void set_active(bool state) { active_ = state; }
    void set_highlight(bool state) { highlight_ = state; }
    void set_position(const math::vector2f& pos) { sprite_.setPosition(pos); hover_bounds_ = sprite_.getGlobalBounds(); }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_ICON_HPP_
