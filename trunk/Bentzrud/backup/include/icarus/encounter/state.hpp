#ifndef ICARUS_ENCOUNTER_STATE_HPP_
#define ICARUS_ENCOUNTER_STATE_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "icarus/base_state.hpp"
#include "character_base.hpp"
#include "grid.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/encounter/data.hpp"
#include "icarus/encounter/skill_handler.hpp"

namespace icarus
{
namespace encounter
{
class state : public base_state
{
private:
    struct cast
    {
        character_hero* caster_;
        unsigned skill_;
        bool active_;
        cast(character_hero* caster, unsigned skill)
        :
            caster_(caster),
            skill_(skill),
            active_(true)
        {}
        cast() : caster_(NULL), skill_(0), active_(false) {}
    };
    encounter::data current_encounter_;
    sf::Sprite* background_;
    grid* grid_;
    character_base* selected_character_;
    sf::CircleShape* selection_indicator_;
    skill_handler* skill_handler_;
    cast active_cast_;

    // temp?
    std::vector<character_base*> enemy_list;
    std::vector<character_base*> player_list;

    void place_character(character_base* character,
                         const math::vector2i& vec_pos) const;
    void cast_player_skill(character_hero* active_char,
                           unsigned skill,
                           grid_tile* target_tile = NULL);
protected:

public:
    state(sf::RenderWindow *const window);
    virtual ~state();
    bool setup_encounter(const encounter::data& new_encounter);

    virtual void pause_state();
    virtual void resume_state();

    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_STATE_HPP_
