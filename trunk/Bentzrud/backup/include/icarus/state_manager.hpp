#ifndef ICARUS_STATE_MANAGER_HPP_
#define ICARUS_STATE_MANAGER_HPP_

#include <vector>
#include "base_state.hpp"
#include "state_data.hpp"
#include "config.hpp"

namespace icarus
{
class state_manager
{
private:
    std::vector<base_state*> state_stack_;
    int delta_;
    int prev_delta_;
    sf::Clock game_time_;
    state_data* stored_data_;
    sf::RenderWindow* window_;
    //config config_;

    state_manager();
    state_manager(state_manager const&);
    void operator=(state_manager const&);
    void run_game();
    void take_screenshot();

public:
    ~state_manager();
    static state_manager& get_instance()
    {
        static state_manager instance;
        return instance;
    }

    void start(base_state* initial_state, sf::RenderWindow* window);
    void change_state(base_state* new_state);
    void push_state(base_state* new_state);
    void pop_state();

    void store_data(state_data* new_data);
    state_data* retrieve_data();
    double get_delta() const;
};
} // namespace icarus

#endif // ICARUS_STATE_MANAGER_HPP_
