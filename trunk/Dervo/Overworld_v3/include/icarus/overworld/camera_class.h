#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "statistics.hpp"
#include <iostream>
enum coordinate{cord_height, cord_width};
enum direction{upwards, downwards, leftwards, rightwards};

namespace icarus
{
namespace overworld
{
class camera_class
{
private:
    math::vector2f      old_mouse_pos_;
    math::vector2f      new_mouse_pos_;
    sf::RenderWindow*   window_;
    statistics          stats;
    sf::Vector2f        camera_center;
    float               max_height,
                        min_height,
                        max_width,
                        min_width;
    sf::View            active_view;
protected:
public:
    camera_class();
    virtual ~camera_class();

    void setup(math::vector2f scenter, sf::RenderWindow *window);
    void update(const std::vector<sf::Event>& key_events);
    bool is_inside_bounds(direction way);
    void move_in_direction(direction way, float distance);
    sf::View* return_view();
};

} // namespace overworld
} // namespace icarus
#endif // CAMERA_CLASS_H
