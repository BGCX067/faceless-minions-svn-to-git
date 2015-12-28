#ifndef ICARUS_INPUT_HANDLER_HPP_
#define ICARUS_INPUT_HANDLER_HPP_

#include <SFML/Graphics.hpp>

#include "icarus/math/vector2.hpp"

namespace icarus
{
class input_handler
{
private:
    sf::RenderWindow *window_;
    input_handler();
    input_handler(input_handler const&);
    void operator=(input_handler const&);
    static input_handler* instance_;
public:
    ~input_handler();
    static input_handler* get()
    {
        if (!instance_)
            instance_ = new input_handler();
        return instance_;
    }
    bool setup_input(sf::RenderWindow *window);
    math::vector2i get_mouse_xy();
    bool get_mouse_button(sf::Mouse::Button button);
    bool get_key_down(sf::Keyboard::Key key);
};
} // namespace icarus

#endif // ICARUS_INPUT_HANDLER_HPP_
