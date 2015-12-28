#ifndef ICARUS_ENDGAME_STATE_HPP_
#define ICARUS_ENDGAME_STATE_HPP_

#include "icarus/base_state.hpp"

namespace icarus
{
namespace endgame
{
class state : public icarus::base_state
{
private:
    sf::Text game_over_;
    sf::Text thx_for_playing_;
    sf::Text return_to_menu_;
    struct fadeable
    {
        double t_max_;
        double t_current_;
        sf::RectangleShape shape_;
    };
    fadeable black_screen_;
    bool visible_;

    uint8_t fade_out(fadeable& shape, double delta);

public:
    state(sf::RenderWindow *const window);
    virtual ~state();

    virtual void pause_state();
    virtual void resume_state();

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
}   // namespace endgame
}   // namespace icarus

#endif // ICARUS_ENDGAME_STATE_HPP_
