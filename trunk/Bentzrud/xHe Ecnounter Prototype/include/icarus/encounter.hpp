#ifndef ICARUS_ENCOUNTER_HPP_
#define ICARUS_ENCOUNTER_HPP_

#include <string>
#include <stdint.h>
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace encounter
{
namespace scenario
{
enum type
{
    undefined,
    forest,
    mountain,
    coast,
    hills
};
} // namespace scenario

struct path_node
{
    int f, g, h;
    math::vector2i vec_pos;
    path_node* parent;
    path_node(math::vector2i v_pos)
    {
        f = g = h = 0;
        parent = NULL;
        vec_pos = v_pos;
    }
    void update_f()
    {
        f = g + h;
    }
};

namespace skill_type
{
enum type
{
    undefined,
    instant,
    projectile,
    beam
};
} // namespace skill_type

namespace effect_type
{
enum type
{
    undefined,
    none,
    bleeding,
    burning,
    frozen,
    chilled
};
}

namespace skill_flags
{
enum value
{
    CAN_DODGE = 0x01,
    CAN_BLOCK = 0x02,
    CAN_RESIST = 0x04,
    IS_CRITICAL = 0x08,
    IS_MAGIC = 0x10,
    USES_WEAPON_DMG = 0x20,
    CAN_AUTOCAST = 0x40
};
}

struct applied_effect
{
    std::string name;
    uint8_t flags;
    int damage;
    float crit_chance_mod;
    float duration;
    unsigned ticks;
    int str_mod, int_mod, agi_mod;
    float speed_mod;
    float damage_mod;
    effect_type::type type;
    applied_effect() :
        name(""), flags(0x0), damage(0), crit_chance_mod(0.0f),
        duration(0.0f), ticks(0), str_mod(0),
        int_mod(0), agi_mod(0), speed_mod(0), damage_mod(0),
        type(effect_type::undefined) {}
};

struct targeted_skill
{
    std::string name;
    uint8_t flags;
    int damage;
    float crit_chance_mod;
    unsigned range;
    skill_type::type type;
    unsigned radius;
    std::string projectile;
    applied_effect effect;
    targeted_skill() :
        name(""), flags(0x0), damage(0),
        crit_chance_mod(0.0f), range(0),
        type(skill_type::undefined), radius(0),
        projectile("") {}
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_HPP_
