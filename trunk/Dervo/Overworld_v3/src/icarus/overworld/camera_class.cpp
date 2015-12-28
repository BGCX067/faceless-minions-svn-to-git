#include "icarus/overworld/camera_class.h"
namespace icarus
{
namespace overworld
{
camera_class::camera_class()
{
    //ctor
}

camera_class::~camera_class()
{
    //dtor
}

void camera_class::setup(math::vector2f scenter, sf::RenderWindow* window)

{
    camera_center = scenter.get_sf_vector();

    max_height = stats.screen_height_;
    min_height = 0;
    max_width  = stats.screen_width_;
    min_width  = 0;
    window_ = window;
    //satan
    //scenter

    sf::Vector2f center(scenter.x, scenter.y);
    sf::Vector2f half_size(stats.screen_width_, stats.screen_height_);
    active_view.setCenter(center);
    active_view.setSize(half_size);

    window_->setView(active_view);


}
void camera_class::update(const std::vector<sf::Event>& key_events)
{
    for (unsigned i = 0; i < key_events.size(); ++i)
    {
        switch (key_events[i].type)
        {
        case sf::Event::KeyPressed:
            if (key_events[i].key.code == sf::Keyboard::Up)
            {
                if(is_inside_bounds(direction::upwards))
                {
                    std::cout << "Up" << std::endl;
                    move_in_direction(direction::upwards, 5);
                }
            }
            else if (key_events[i].key.code == sf::Keyboard::Down)
            {
                if(is_inside_bounds(direction::downwards))
                {
                    std::cout << "Down" << std::endl;
                    move_in_direction(direction::downwards, 5);
                }
            }
            if (key_events[i].key.code == sf::Keyboard::Left)
            {
                if(is_inside_bounds(direction::leftwards))
                {
                    std::cout << "Left" << std::endl;
                    move_in_direction(direction::leftwards, 5);
                }
            }
            else if (key_events[i].key.code == sf::Keyboard::Right)
            {
                if(is_inside_bounds(direction::rightwards))
                {
                    std::cout << "Right" << std::endl;
                    move_in_direction(direction::rightwards, 5);
                }
            }
            break;
        case sf::Event::MouseButtonPressed:
            break;
        case sf::Event::MouseButtonReleased:

            break;
        default: break;
        }
    }
}
bool camera_class::is_inside_bounds(direction way)
{
    /*if(way == direction::rightwards)
    {
        if (camera_center.y <= max_width)
            return true;
    }
    if(way == direction::leftwards)
    {
        if (camera_center.y >= min_width)
            return true;
    }
    if(way == direction::upwards)
    {
        if (camera_center.x >= min_height)
            return true;
    }
    if(way == direction::downwards)
    {
        if (camera_center.x <= max_height)
            return true;
    }
    */
    return true;
    return false;
}
void camera_class::move_in_direction(direction way, float distance)
{
    if(way == direction::downwards)
        active_view.move(0, distance);
    else if(way == direction::upwards )
        active_view.move(0, -distance);
    else if(way == direction::leftwards )
        active_view.move(-distance, 0);
    else if(way == direction::rightwards )
        active_view.move(distance, 0);
    window_->setView(active_view);
}
sf::View* camera_class::return_view()
{
    return &active_view;
}

} // namespace overworld
} // namespace icarus
