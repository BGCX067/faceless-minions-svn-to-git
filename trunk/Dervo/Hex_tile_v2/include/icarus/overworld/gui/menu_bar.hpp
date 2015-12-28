#ifndef ICARUS_OVERWORLD_GUI_MENU_BAR_HPP_
#define ICARUS_OVERWORLD_GUI_MENU_BAR_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/encounter/animate.hpp"
#include "icarus/utils.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class menu_bar : public sf::Drawable
{
private:
    sf::Sprite background_;
    struct button
    {
        sf::Sprite graphic_;
        sf::Text text_;
        bool hover_;
        utils::rgb hover_color_;
        utils::rgb normal_color_;
        button(uint32_t normal,
               uint32_t hover)
        :
            hover_(false),
            hover_color_(hover),
            normal_color_(normal)
        {}
        bool contains(const math::vector2f& pos) const
        {
            return graphic_.getGlobalBounds().contains(pos);
        }
        void set_hover(bool state)
        {
            if (hover_ == state)
            {
                return;
            }
            else
            {
                text_.setColor(state?hover_color_:normal_color_);
                hover_ = state;
            }
        }
        void set_position(const math::vector2f& pos)
        {
            graphic_.setPosition(pos);
            text_.setPosition(pos+math::vector2f(15.0f, 3.0f));
        }
    };
    button party_button_;
    button stats_button_;
    button options_button_;

public:
    menu_bar();
    virtual ~menu_bar();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void handle_input(const std::vector<sf::Event>& input_events);
    bool contains(const math::vector2f& mouse_pos) const;
    void reset_position();
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_MENU_BAR_HPP_
