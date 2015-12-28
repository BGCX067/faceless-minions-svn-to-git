#ifndef ICARUS_STATE_MANAGER_HPP_
#define ICARUS_STATE_MANAGER_HPP_

#include <vector>
#include <windows.h>
#include "icarus/base_state.hpp"
#include "icarus/state_data.hpp"
#include "icarus/utils/yth_handler.hpp"
#include "icarus/math/vector2.hpp"


namespace icarus
{
namespace states
{
enum state
{
    intro,
    menu,
    overworld,
    encounter,
    endgame,
    options
};
}
namespace debug
{
enum value
{
    SHOW_FPS = 0x1,
    SHOW_TIMINGS = 0x2,
    SHOW_PROFILER = 0x4
};
}
namespace encounter // fwd declaration
{
    class animate;
}
namespace state_actions
{
enum value
{
    push,
    pop,
    change,
    quit
};
}
struct state_event
{
    state_actions::value action_;
    states::state state_;
};
class config;
class state_manager
{
private:
    const int BIG_ENDIAN_;
    bool new_game_;
    std::vector<base_state*> state_stack_;
    int delta_;
    int prev_delta_;
    sf::Clock game_time_;
    state_data* stored_data_;
    sf::RenderWindow* window_;
    states::state initial_state_;
    uint8_t debug_flags_;
    static state_manager* instance_;
    encounter::animate* cursor_;
    std::vector<state_event> state_events_;
    bool full_hd_;
    bool vsync_enabled_;
    bool is_fullscreen_;
    bool hd_is_supported_;
    struct debug_data_
    {
        double events_, input_, logic_, draw_;
        sf::Text t_events_, t_input_, t_logic_, t_draw_, t_profiler_;
        int frame_counter_;
        double debug_timer_;
        debug_data_()
        : events_(0.0), input_(0.0), logic_(0.0), draw_(0.0),
          frame_counter_(0), debug_timer_(0.0)
        {
            t_events_.setCharacterSize(10);
            t_input_.setCharacterSize(10);
            t_logic_.setCharacterSize(10);
            t_draw_.setCharacterSize(10);
            t_profiler_.setCharacterSize(10);
        }
        void reset()
        {
            events_ = input_ = logic_ = draw_ = debug_timer_ = 0.0;
            frame_counter_ = 0;
        }
    } debug_data_;

    state_manager();
    state_manager(state_manager const&);
    void operator=(state_manager const&);
    void run_game();
    int take_screenshot();
    void update_debug_flags(const sf::Event& event);
    void draw_debug();

    void change(states::state new_state);
    void push(states::state new_state);
    void pop();
    void quit();
    void save_config();

    config* config_;
    utils::yth_handler yth_handler_;

public:
    ~state_manager();
    static state_manager* get()
    {
        if (!instance_)
            instance_ = new state_manager();
        return instance_;
    }

    void initialize(const std::string& config_path);
    void start();

    void change_state(states::state new_state);
    void push_state(states::state new_state);
    void pop_state();
    void quit_game();
    void save_game(base_state* state = NULL);
    void load_game(base_state* state = NULL);

    void store_data(state_data* new_data);
    state_data* retrieve_data();
    double get_delta() const;
    void set_cursor(const std::string& type);
    void set_debug_profiler(double value);
    void reset_cursor_pos();
    void set_window_options(bool fullscreen,
                            bool hd,
                            bool vsync);

    double get_game_time() const
    {
        return game_time_.getElapsedTime().asMicroseconds();
    }
    int get_seconds() const
    {
        return game_time_.getElapsedTime().asSeconds();
    }
    int get_milliseconds() const
    {
        return game_time_.getElapsedTime().asMilliseconds();
    }
    math::vector2f get_dimensions() const
    {
        return math::vector2f(sf::Vector2f(window_->getSize()));
    }
    bool get_hd_status() const { return full_hd_; }
    bool is_new_game() const { return new_game_; }
    void set_new_game(bool value) { new_game_ = value; }
    bool get_fullscreen() const { return is_fullscreen_; }
    bool get_vsync() const { return vsync_enabled_; }
    bool is_hd_supported() const { return hd_is_supported_; }
    void print_err(const std::string& title, const std::string& message)
    {
        MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
    }
};
} // namespace icarus

#endif // ICARUS_STATE_MANAGER_HPP_
