#ifndef ICARUS_ENCOUNTER_STATE_HPP_
#define ICARUS_ENCOUNTER_STATE_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "character.hpp"
#include "grid.hpp"
#include "icarus/input_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace scenario
{
enum type
{
    forest,
    mountain,
    coast,
    hills
};
} // namespace scenario

// strictly temporary
struct encounter_data
{
    int wave_count;
    int enemies_per_wave;
    scenario::type active_scenario;
};

class state
{
private:
    encounter_data current_encounter_;
    sf::RenderWindow *const window_;
    sf::Sprite* background_;
    grid *grid_;
    character* selected_character_;
    sf::CircleShape* selection_indicator_;

    // temp?
    std::vector<character*> enemy_list;
    std::vector<character*> player_list;

protected:

public:
    state(sf::RenderWindow *const window);
    ~state();
    bool setup_encounter(const encounter_data& new_encounter);
    void handle_input();
    void handle_logic();
    void draw() const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_STATE_HPP_
