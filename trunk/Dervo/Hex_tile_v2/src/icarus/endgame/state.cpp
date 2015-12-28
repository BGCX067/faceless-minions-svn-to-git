#include "icarus/endgame/state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/utils.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/state_manager.hpp"

namespace icarus
{
namespace endgame
{
state::state(sf::RenderWindow *const window)
:
    base_state(window),
    visible_(false)
{
    game_over_.setString("Game Over");
    game_over_.setCharacterSize(60);
    game_over_.setStyle(sf::Text::Bold);
    game_over_.setColor(utils::rgb(0xFF0000));
    game_over_.setFont(*resource_handler::get()->get_font("numbers"));
    game_over_.setPosition(math::vector2f((window_->getSize().x - game_over_.getLocalBounds().width) / 2,
                                          15.0f));

    thx_for_playing_.setString("Thanks for playing!");
    thx_for_playing_.setCharacterSize(16);
    thx_for_playing_.setFont(*resource_handler::get()->get_font("text"));
    thx_for_playing_.setPosition(math::vector2f((window_->getSize().x - thx_for_playing_.getLocalBounds().width) / 2,
                                                game_over_.getPosition().y + game_over_.getLocalBounds().height + 15.0f));

    return_to_menu_.setString("Return to Menu...");
    return_to_menu_.setCharacterSize(25);
    return_to_menu_.setFont(*resource_handler::get()->get_font("text"));
    return_to_menu_.setPosition(math::vector2f(10.0f, window_->getSize().y - return_to_menu_.getLocalBounds().height - 10.0f));

    black_screen_.shape_.setFillColor(utils::rgb(0x000000));
    black_screen_.shape_.setPosition(0.0f, 0.0f);
    black_screen_.shape_.setSize(math::vector2f(window_->getSize().x, window_->getSize().y));
    black_screen_.t_current_ = black_screen_.t_max_ = 3.0;
}
state::~state()
{
}

uint8_t state::fade_out(fadeable& shape, double delta)
{
    if (shape.t_current_ <= 0.0)
        return 0;
    shape.t_current_ -= delta;
    sf::Color new_color = shape.shape_.getFillColor();
    uint8_t amount = static_cast<uint8_t>(255.0 * (shape.t_current_ / shape.t_max_));
    new_color.a = math::clamp(amount, static_cast<uint8_t>(0), static_cast<uint8_t>(255));
    shape.shape_.setFillColor(new_color);
    return new_color.a;
}

void state::pause_state()
{

}
void state::resume_state()
{

}

void state::handle_input(const std::vector<sf::Event>& key_events)
{
    if (visible_)
    {
        for (unsigned i = 0; i < key_events.size(); ++i)
        {
            switch(key_events[i].type)
            {
            case sf::Event::MouseButtonReleased:
            {
                math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseButton.x,
                                                                                                  key_events[i].mouseButton.y));
                if (return_to_menu_.getGlobalBounds().contains(mouse_pos))
                {
                    state_manager::get()->change_state(states::menu);
                    return;
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseMove.x,
                                                                                                  key_events[i].mouseMove.y));
                if (return_to_menu_.getGlobalBounds().contains(mouse_pos))
                {
                    return_to_menu_.setColor(utils::rgb(0xFFFF00));
                }
                else
                {
                    return_to_menu_.setColor(utils::rgb(0xFFFFFF));
                }
                break;
            }
            default: break;
            }
        }
    }
}
void state::handle_logic()
{
    double delta = state_manager::get()->get_delta();
    if (!visible_)
        if (fade_out(black_screen_, delta) == 0)
            visible_ = true;
}
void state::draw() const
{
    window_->draw(game_over_);
    window_->draw(thx_for_playing_);
    window_->draw(return_to_menu_);

    if (!visible_)
        window_->draw(black_screen_.shape_);
}
}   // namespace endgame
}   // namespace icarus
