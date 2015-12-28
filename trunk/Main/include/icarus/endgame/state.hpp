#ifndef ICARUS_ENDGAME_STATE_HPP_
#define ICARUS_ENDGAME_STATE_HPP_

#include <queue>
#include "icarus/base_state.hpp"
#include "icarus/state_data.hpp"
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace endgame
{
class data : public state_data
{
public:
    data() {}
    virtual ~data() {}
    bool win_;
    std::vector<std::string> graveyard_;
};
class state : public icarus::base_state
{
private:
    bool win_;
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

    struct scroll_text
    {
        float duration_;
        float delay_;
        sf::Text text_;
    };
    std::vector<scroll_text> text_vector_;
    std::queue<scroll_text> text_queue_;

    float text_speed_;
    float text_lifetime_;
    float text_fade_limit_;
    float text_delay_;
    math::vector2f text_spawn_pos_;

    uint8_t fade_out(fadeable& shape, double delta);
    void add_text(const std::string& text, float delay, unsigned f_size);

public:
    state(sf::RenderWindow *const window);
    virtual ~state();

    virtual void pause_state();
    virtual void resume_state();

    virtual void save(std::ofstream& stream) {};
    virtual void load(std::ifstream& stream) {};

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
}   // namespace endgame
}   // namespace icarus

#endif // ICARUS_ENDGAME_STATE_HPP_
