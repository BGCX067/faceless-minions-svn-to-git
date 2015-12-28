#ifndef ICARUS_MENU_BUTTON_HPP_
#define ICARUS_MENU_BUTTON_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/utils.hpp"

namespace icarus
{
namespace menu
{
namespace button_target
{
enum value
{
    undefined = 0,
    intro,
    menu,
    new_game,
    continue_game,
    options,
    quit,
    debug
};
}
class button : public sf::Drawable
{
private:
    sf::Sprite* button_;
    sf::Text label_;
    button_target::value target_;
    bool hover_;
    bool enabled_;
public:
    button(const std::string& button_type);
    virtual ~button();

    void click(sf::RenderWindow* window);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_position(const math::vector2f& new_pos);
    bool contains(const math::vector2f& point);
    void set_hover(bool status);
    bool get_hover() const { return hover_; }
    void set_target(button_target::value target) { target_ = target; }
    void set_enabled(bool state) { enabled_ = state;
    label_.setColor(utils::rgb(!enabled_ ? utils::rgb(0xBBBBBB) : utils::rgb(0xffdd4e))); }
    bool is_enabled() const { return enabled_; }
};
}   // namespace menu
}   // namespace icarus

#endif // ICARUS_MENU_BUTTON_HPP_
