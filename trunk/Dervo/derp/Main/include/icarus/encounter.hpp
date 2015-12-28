#ifndef ICARUS_ENCOUNTER_HPP_
#define ICARUS_ENCOUNTER_HPP_

#include <string>
#include <stdint.h>
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace encounter
{
class character_base;
namespace attribute_mod
{
    const float STR_DAMAGE_MOD = 0.01f;
    const float STR_HEALTH_MOD = 9.0f;
    const float AGI_SPEED_MOD = 0.01f;
    const float AGI_COOLDOWN_MOD = 0.01f;
    const float AGI_DODGE_MOD = 0.1f;
    const float AGI_CRITICAL_MOD = 0.03f;
    const float AGI_RESIST_MOD = 0.01f;
    const float INT_DAMAGE_MOD = 0.01f;
    const float INT_CRITICAL_MOD = 0.2f;
    const float INT_RESIST_MOD = 0.01f;
}
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
    railgun,
    self
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
    CAN_DODGE = 0x0001,
    CAN_BLOCK = 0x0002,
    CAN_RESIST = 0x0004,
    IS_CRITICAL = 0x0008,
    IS_MAGIC = 0x0010,
    USES_WEAPON_DMG = 0x0020,
    CAN_AUTOCAST = 0x0040,
    IS_LEVELED = 0x0080,
    IS_ULTIMATE = 0x0100
};
}
struct stat_mod
{
    float crit_chance_mod_;
    int str_mod_, int_mod_, agi_mod_;
    float speed_mod_;
    float damage_mod_;
    bool stun_;
    stat_mod() :
        crit_chance_mod_(0.0f), str_mod_(0),
        int_mod_(0), agi_mod_(0), speed_mod_(0),
        damage_mod_(0), stun_(false) {}
};

struct applied_effect
{
    std::string name;
    uint16_t flags;
    int damage;
    float crit_chance_mod;
    float duration;
    unsigned ticks;
    int str_mod, int_mod, agi_mod;
    float speed_mod;
    float damage_mod;
    effect_type::type type;
    character_base* source_;
    bool stun_;
    applied_effect() :
        name(""), flags(0x0), damage(0), crit_chance_mod(0.0f),
        duration(0.0f), ticks(0), str_mod(0),
        int_mod(0), agi_mod(0), speed_mod(0), damage_mod(0),
        type(effect_type::undefined), source_(NULL), stun_(false) {}
};

struct targeted_skill
{
    std::string name;
    uint16_t flags;
    int damage;
    float crit_chance_mod;
    unsigned range;
    skill_type::type type;
    unsigned radius;
    std::string projectile;
    applied_effect effect;
    character_base* source_;
    targeted_skill() :
        name(""), flags(0x0), damage(0),
        crit_chance_mod(0.0f), range(0),
        type(skill_type::undefined), radius(0),
        projectile(""), source_(NULL) {}
    void set_source(character_base* source)
    {
        source_ = source;
        effect.source_ = source;
    }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_HPP_
