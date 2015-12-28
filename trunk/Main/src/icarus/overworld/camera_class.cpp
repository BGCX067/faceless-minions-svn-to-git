#include "icarus/overworld/camera_class.h"
#include "icarus/state_manager.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
namespace icarus
{
namespace overworld
{
camera_class::camera_class()
:
    stats(statistics::get()),
    key_move_vector_(0.0f,0.0f),
    mouse_move_vector_(0.0f,0.0f),
    scroll_speed_(300.0f)
{
    key_arr_[0] = false;
    key_arr_[1] = false;
    key_arr_[2] = false;
    key_arr_[3] = false;
    //ctor
}

camera_class::~camera_class()
{
    //dtor
}

void camera_class::setup(math::vector2f scenter, sf::RenderWindow* window)

{
    max_height = stats->screen_height_;
    min_height = 0;
    max_width  = stats->screen_width_;
    min_width  = 0;
    window_ = window;

    sf::Vector2f center(scenter.x, scenter.y);
    sf::Vector2f half_size(stats->screen_width_, stats->screen_height_);
    active_view.setCenter(center);
    active_view.setSize(half_size);

    window_->setView(active_view);

    center = sf::Vector2f(window_->getSize().x / 2, window_->getSize().y / 2);
    half_size = sf::Vector2f(window_->getSize().x, window_->getSize().y);
    encounter_view_.setCenter(center);
    encounter_view_.setSize(half_size);
}

void camera_class::reset_view()
{
    sf::Vector2f center = sf::Vector2f(window_->getSize().x / 2, window_->getSize().y / 2);
    sf::Vector2f half_size = sf::Vector2f(window_->getSize().x, window_->getSize().y);
    encounter_view_.setCenter(center);
    encounter_view_.setSize(half_size);
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
            switch (key_events[i].key.code)
            {
            case sf::Keyboard::Up: key_arr_[0] = true; break;
            case sf::Keyboard::Down: key_arr_[1] = true; break;
            case sf::Keyboard::Left: key_arr_[2] = true; break;
            case sf::Keyboard::Right: key_arr_[3] = true; break;
            default:break;
            }
            break;
        case sf::Event::KeyReleased:
            switch (key_events[i].key.code)
            {
            case sf::Keyboard::Up: key_arr_[0] = false; break;
            case sf::Keyboard::Down: key_arr_[1] = false; break;
            case sf::Keyboard::Left: key_arr_[2] = false; break;
            case sf::Keyboard::Right: key_arr_[3] = false; break;
            default:break;
            }
            break;
        case sf::Event::MouseMoved:
        {
            math::vector2f bounds = state_manager::get()->get_dimensions();
            math::vector2f mouse_pos = math::vector2f(key_events[i].mouseMove.x, key_events[i].mouseMove.y);
            if (mouse_pos.x <= 3.0f)
                mouse_move_vector_.x = -1.0f;
            else if (mouse_pos.x >= bounds.x - 3.0f)
                mouse_move_vector_.x = 1.0f;
            else
                mouse_move_vector_.x = 0.0f;

            if (mouse_pos.y <= 3.0f)
                mouse_move_vector_.y = -1.0f;
            else if (mouse_pos.y >= bounds.y - 3.0f)
                mouse_move_vector_.y = 1.0f;
            else
                mouse_move_vector_.y = 0.0f;
            break;
        }
        default: break;
        }
    }
    if (key_arr_[0] && !key_arr_[1])
        key_move_vector_.y = -1.0f;
    else if (!key_arr_[0] && key_arr_[1])
        key_move_vector_.y = 1.0f;
    else
        key_move_vector_.y = 0.0f;

    if (key_arr_[2] && !key_arr_[3])
        key_move_vector_.x = -1.0f;
    else if (!key_arr_[2] && key_arr_[3])
        key_move_vector_.x = 1.0f;
    else
        key_move_vector_.x = 0.0f;

    if ((key_move_vector_.is_nonzero() || mouse_move_vector_.is_nonzero()) &&
        gui::gui_handler::get()->scrolling_enabled())
    {
        double delta = state_manager::get()->get_delta();
        active_view.move((key_move_vector_.is_nonzero() ?
                          key_move_vector_.normalized() :
                          mouse_move_vector_.normalized()) * scroll_speed_ * delta);
        window_->setView(active_view);
        state_manager::get()->reset_cursor_pos();
        gui::gui_handler::get()->reset_position();
    }
}
sf::View* camera_class::return_view()
{
    return &active_view;
}
void camera_class::set_encounter_view(bool active)
{
    if (active)
        window_->setView(encounter_view_);
    else
        window_->setView(active_view);
}
void camera_class::center_view(const math::vector2f& center)
{
    active_view.setCenter(center);
    window_->setView(active_view);
}
} // namespace overworld
} // namespace icarus
