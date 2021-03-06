#ifndef OVERWORLD_STATE_H
#define OVERWORLD_STATE_H

#include <SFML/Graphics.hpp>
#include "icarus/base_state.hpp"
#include "icarus/overworld/hex_grid_creator.hpp"
#include "icarus/overworld/nation_creator.hpp"
#include "icarus/overworld/characacter_holder.hpp"
#include "icarus/overworld/camera_class.h"
#include <iostream>

#include "icarus/overworld/statistics.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
namespace icarus
{
namespace overworld
{
class overworld_state : public base_state
{
private:
    hex_grid_creator                    grid_creator_;
    nation_creator                      nations_;
    statistics*                          stats;
    characacter_holder                  character_;
    camera_class                        camera_;
    gui::gui_handler*                   gui_;

    typedef std::vector<hex_sprite>     hex_hold_;
    std::vector<hex_hold_>              hex_grid_;

    typedef std::vector <hex_sprite*>   nation_group_;
    std::vector<nation_group_>          nation_tiles_;
    std::vector<nation_group_>          nature_tiles_;

    std::vector<nation_info>            nations_information_;

    utils::yth_node*                    encounter_pointer;// use this

    bool                                togle_fow;
    unsigned                            konami_[10];
    unsigned                            konami_index_;
    unsigned                            round_count_;
protected:

public:
    overworld_state(sf::RenderWindow *const window);
    virtual ~overworld_state();
    bool setup_overworld();

    virtual void pause_state();
    virtual void resume_state();
    virtual void save(std::ofstream& stream);
    virtual void load(std::ifstream& stream);

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
} // namespace overworld
} // namespace icarus

#endif // OVERWORLD_STATE_H
