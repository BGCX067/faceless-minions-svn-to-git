#ifndef OVERWORLD_STATE_H
#define OVERWORLD_STATE_H

#include <SFML/Graphics.hpp>
#include "icarus/base_state.hpp"
#include "hex_grid_creator.hpp"
#include "nation_creator.hpp"
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
    nation_creator nations_;
    statistics       stats;
    characacter_holder character_;

    typedef std::vector<hex_sprite>     hex_hold_;
    std::vector<hex_hold_>              hex_grid_;

    typedef std::vector <hex_sprite*>   nation_group_;
    std::vector<nation_group_>          nation_tiles_;
    std::vector<nation_group_>          nature_tiles_;

    std::vector<nation_info>            nations_information_;


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
