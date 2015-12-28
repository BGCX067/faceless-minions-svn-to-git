#ifndef ICARUS_STATE_MANAGER_HPP_
#define ICARUS_STATE_MANAGER_HPP_

#include <vector>
#include "icarus/base_state.hpp"
#include "icarus/state_data.hpp"
#include "icarus/config.hpp"
#include "icarus/utils/yth_handler.hpp"


namespace icarus
{
namespace states
{
enum state
{
    intro,
    menu,
    overworld,
    encounter
};
}
namespace debug
{
enum value
{
    SHOW_FPS = 0x1
};
}
namespace encounter // fwd declaration
{
    class animate;
}
class state_manager
{
private:
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

    state_manager();
    state_manager(state_manager const&);
    void operator=(state_manager const&);
    void run_game();
    void take_screenshot();
    void update_debug_flags(const sf::Event& event);
    void draw_debug();

    config config_;
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

    void change_state(base_state* new_state);
    void push_state(base_state* new_state);
    void pop_state();

    void store_data(state_data* new_data);
    state_data* retrieve_data();
    double get_delta() const;
    void set_cursor(const std::string& type);
};
} // namespace icarus

#endif // ICARUS_STATE_MANAGER_HPP_
