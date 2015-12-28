#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "statistics.hpp"
#include <iostream>

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
    statistics*          stats;
    sf::Vector2f        camera_center;
    float               max_height,
                        min_height,
                        max_width,
                        min_width;
    sf::View            active_view;
    sf::View            encounter_view_;
    bool                key_arr_[4];
    math::vector2f      key_move_vector_;
    math::vector2f      mouse_move_vector_;
    float               scroll_speed_;
protected:
public:
    camera_class();
    virtual ~camera_class();

    void setup(math::vector2f scenter, sf::RenderWindow *window);
    void update(const std::vector<sf::Event>& key_events);
    sf::View* return_view();
    void set_encounter_view(bool active);
    void center_view(const math::vector2f& center);
    void reset_view();
};

} // namespace overworld
} // namespace icarus
#endif // CAMERA_CLASS_H
