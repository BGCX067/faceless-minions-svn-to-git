#ifndef ICARUS_UTILS_SERIALIZABLE_HPP_
#define ICARUS_UTILS_SERIALIZABLE_HPP_

#include <fstream>

namespace icarus
{
namespace utils
{
namespace encounter_state
{
enum value
{
    no_encounter,
    enter_encounter,
    choice_made,
    encounter_start,
    encounter_in_progress,
    encounter_resolved
};
}
union grid_tile
{
    char bytes[4];
    struct
    {
        uint32_t ground_:3,
                owner_:2,
                overlay_:5,
                visible_:1,
                delay_:4,
                battlezone_:1,
                attacker_:2,
                unused_:14;
    };
};
union character
{
    char bytes[24];

    struct
    {
        uint32_t    active_:1,
                    strength_:7,
                    experience_:24,
                    gender_:1,
                    intelligence_:7,
                    damage_done_:24,
                    first_name_:9,
                    battle_count_:16,
                    agility_:7,
                    surname_:10,
                    kills_:16,
                    class_:3,
                    skill0_:2,
                    skill3_:1,
                    damage_taken_:24,
                    skill1_:2,
                    skill2_:2,
                    autocast_:4,
                    assists_:16,
                    current_hp_:11,
                    filler_:5;
    };
};
union item
{
    char bytes[18];

    struct
    {
        uint16_t    active_:1,
                    base_:8,
                    dmgmod_:7,
                    unique_:1,
                    prefix_:8,
                    str_:7,
                    type_:2,
                    int_:7,
                    agi_:7,
                    critchance_:5,
                    dodgechance_:5,
                    hpregen_:6,
                    postfix_:8,
                    critmod_:5,
                    filler1_:3,
                    mindmg_:10,
                    blockchance_:5,
                    filler2_:1,
                    maxdmg_:10,
                    blockmod_:5,
                    filler3_:1,
                    bonusdmg_:10,
                    reschance_:5,
                    filler4_:1,
                    hpbonus_:10,
                    resmod_:5,
                    filler5_:1;
    };
};
union current_state
{
    char bytes[6];
    struct
    {
        uint16_t    state_:3,
                    region_:8,
                    encounter_:8,
                    selected_:3,
                    scenario_:3,
                    party_xp_:11,
                    result_:1,
                    unused_:11;
    };
};
union party_member
{
    char bytes[14];
    struct
    {
        uint16_t    active_:1,
                    current_hp_:11,
                    kills_:8,
                    assists_:8,
                    dmg_done_:16,
                    dmg_taken_:16,
                    largest_given_:16,
                    largest_taken_:16,
                    friendly_fire_:16,
                    unused_:4;
    };
};
class serializable
{
public:
    serializable() {}
    virtual ~serializable() {}
    virtual void save(std::ofstream& stream) = 0;
    virtual void load(std::ifstream& stream) = 0;
};
}
}

#endif // ICARUS_UTILS_SERIALIZABLE_HPP_
