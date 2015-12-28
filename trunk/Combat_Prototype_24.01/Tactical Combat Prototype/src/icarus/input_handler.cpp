#include "icarus/input_handler.hpp"

namespace icarus
{
input_handler::input_handler() :
    window_(NULL)
{
}

input_handler::~input_handler()
{
}

bool input_handler::setup_input(sf::RenderWindow *window)
{
    window_ = window;
    return true;
}

math::vector2i input_handler::get_mouse_xy()
{
    return math::vector2i(sf::Mouse::getPosition(*window_));
}

bool input_handler::get_mouse_button(sf::Mouse::Button button)
{
    return sf::Mouse::isButtonPressed(button);
}

bool input_handler::get_key_down(sf::Keyboard::Key key)
{
    return sf::Keyboard::isKeyPressed(key);
}
} // namespace icarus
