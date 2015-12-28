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
    template<typename T>
    math::vector2<T> get_mouse_xy()
    {
        sf::Vector2f temp = window_->mapPixelToCoords(sf::Mouse::getPosition(*window_));
        return math::vector2<T>(temp.x, temp.y);
    }
    bool get_mouse_button(sf::Mouse::Button button);
    bool get_key_down(sf::Keyboard::Key key);
    template<typename T>
    math::vector2<T> convert_mouse_pos(const math::vector2<T>& pos)
    {
        sf::Vector2f temp = window_->mapPixelToCoords(sf::Vector2i(static_cast<int>(pos.x),
                                                                static_cast<int>(pos.y)));
        return math::vector2<T>(static_cast<T>(temp.x),
                                static_cast<T>(temp.y));
    }
    template<typename T>
    math::vector2<T> pixel_to_coords(const math::vector2<T>& pos)
    {
        sf::Vector2i temp = window_->mapCoordsToPixel(sf::Vector2f(static_cast<int>(pos.x),
                                                                static_cast<int>(pos.y)));
        return math::vector2<T>(static_cast<T>(temp.x),
                                static_cast<T>(temp.y));
    }
};
} // namespace icarus

#endif // ICARUS_INPUT_HANDLER_HPP_
