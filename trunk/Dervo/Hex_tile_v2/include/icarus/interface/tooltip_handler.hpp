#ifndef ICARUS_INTERFACE_TOOLTIP_HANDLER_HPP_
#define ICARUS_INTERFACE_TOOLTIP_HANDLER_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/interface/base_tooltip.hpp"
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace interface
{
class tooltip_handler : public sf::Drawable
{
private:
    static tooltip_handler* instance_;
    sf::RectangleShape frame_;
    std::vector<sf::Text> text_elements_;
    math::vector2f position_;
    float text_spacing_;

    tooltip_handler();
    tooltip_handler(tooltip_handler const&);
    void operator=(tooltip_handler const&);
public:
    virtual ~tooltip_handler();
    static tooltip_handler* get()
    {
        if (!instance_)
            instance_ = new tooltip_handler();
        return instance_;
    }

    void update_tooltip(const sf::Event& move_event);
    void create_tooltip(base_tooltip* tooltip_ptr);
    void set_status_all(bool status);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
}   // namespace interface
}   // namespace icarus

#endif // ICARUS_INTERFACE_TOOLTIP_HANDLER_HPP_
