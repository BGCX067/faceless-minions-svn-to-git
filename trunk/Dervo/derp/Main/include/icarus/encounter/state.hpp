#ifndef ICARUS_ENCOUNTER_STATE_HPP_
#define ICARUS_ENCOUNTER_STATE_HPP_

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "icarus/base_state.hpp"
#include "icarus/encounter/character_hero.hpp"
#include "icarus/encounter/character_npc.hpp"
#include "icarus/encounter/grid.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/encounter/data.hpp"
#include "icarus/encounter/skill_handler.hpp"
#include "icarus/encounter/ui/skillbar.hpp"
#include "icarus/encounter/obstacle.hpp"

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
    encounter::data* current_encounter_;
    sf::Sprite* background_;
    grid* grid_;
    character_hero* selected_character_;
    sf::CircleShape* selection_indicator_;
    skill_handler* skill_handler_;
    ui::skillbar* skill_bar_;
    cast active_cast_;
    unsigned current_wave_;
    unsigned wave_count_;
    unsigned enemies_remaining_;
    unsigned enemies_total_;
    utils::countdown wave_timer_;

    typedef std::vector<character_npc*> enemy_vector;
    std::vector<enemy_vector> waves_;

    std::vector<character_hero*> player_list_;
    std::vector<obstacle*> obstacles_;
    std::vector<character_base*> depth_sorting_;

    void place_character(character_base* character,
                         const math::vector2i& vec_pos) const;
    void untranslated_place(character_base* character,
                            const math::vector2i& vec_pos) const;

    void cast_ai_skills();
    void start_next_scenario();
    void update_depth_sort();
    void exit_encounter();
    void find_autocast_target(unsigned player_index,
                              unsigned skill_index);
    void set_selected_char(unsigned index);
    struct depth_sort
    {
        inline bool operator()(const character_base* a, const character_base* b)
        {
            return (a->get_y_depth() < b->get_y_depth());
        }
    };

protected:

public:
    state(sf::RenderWindow *const window);
    virtual ~state();
    bool setup_encounter(encounter::data* new_encounter);

    virtual void pause_state();
    virtual void resume_state();

    void cast_player_skill(character_hero* active_char,
                           unsigned skill,
                           grid_tile* target_tile = NULL);
    virtual void handle_input(const std::vector<sf::Event>& key_events);
    virtual void handle_logic();
    virtual void draw() const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_STATE_HPP_
