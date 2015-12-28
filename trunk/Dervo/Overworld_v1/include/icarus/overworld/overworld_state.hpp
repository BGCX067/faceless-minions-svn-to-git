#ifndef OVERWORLD_STATE_H
#define OVERWORLD_STATE_H

#include <SFML/Graphics.hpp>
#include "icarus/base_state.hpp"
#include "hex_grid_creator.hpp"
#include "characacter_holder.hpp"
#include <iostream>

#include "statistics.hpp"
namespace icarus
{
namespace overworld
{
class overworld_state : public base_state
{
private:
    hex_grid_creator grid_creator_;
    statistics       stats;
    characacter_holder character_;
protected:

public:
    overworld_state(sf::RenderWindow *const window);
    virtual ~overworld_state();
    bool setup_overworld();

    virtual void pause_state();
    virtual void resume_state();

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
} // namespace overworld
} // namespace icarus

#endif // OVERWORLD_STATE_H
