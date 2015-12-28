#include <cstdlib>
#include <cmath>
#include "icarus/overworld/party_member.hpp"
#include "icarus/math.hpp"
#include "icarus/encounter.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace overworld
{
party_member::party_member(unsigned index,
                           const std::string& name,
                           const std::string& surname,
                           encounter::hero_class::type char_class,
                           unsigned level)
:
    party_index_(index),
    name_(name),
    surname_(surname),
    base_health_(0),
    current_health_(0),
    agility_(0),
    strength_(0),
    intelligence_(0),
    move_speed_(0.0f),
    level_(math::clamp(level, unsigned(1), unsigned(10))),
    experience_(0),
    total_kills_(0),
    total_assists_(0),
    total_dmg_done_(0),
    total_dmg_taken_(0),
    largest_hit_done_(0),
    largest_hit_taken_(0),
    total_ff_done_(0),
    total_battles_(0),
    unspent_skill_pts_(0),
    unspent_attrib_pts_(0),
    class_(char_class),
    level_exponent_(267.07f)
{
    //TODO:: generate appropriate stats based on level
    base_health_ = 100;
    current_health_ = base_health_;

    unspent_attrib_pts_ = 5 * level_;
    for (unsigned i = 0; i < level_; ++i) // for each level
    {
        for (unsigned k = 0; k < 5; ++k)    // assign 5 attribute points
        {
            spend_attrib_point(int(rand()%3));
        }
        unspent_skill_pts_++;
    }
    // TODO:: fix proper generation for other stats
    move_speed_ = 250.0f;

    autocast_[0] = true;
    autocast_[1] = false;
    autocast_[2] = false;
    autocast_[3] = false;
    autocast_[4] = false;

    skill_levels_[0] = 0;
    skill_levels_[1] = 0;
    skill_levels_[2] = 0;
    skill_levels_[3] = 0;

    for (unsigned i = 0; i < 10; ++i)
    {
        xp_requirements_[i] = ceil(level_exponent_ * pow(1.5f, i) - level_exponent_);
    }

    experience_ = xp_requirements_[level_ - 1];
}
party_member::~party_member()
{
}

void party_member::generate_hero_data(encounter::hero_data& data) const
{
    data.index_ = party_index_;
    data.name_ = name_;
    data.surname_ = surname_;
    for (unsigned i = 0; i < 4; ++i)
        data.skill_levels_[i] = skill_levels_[i];
    for (unsigned i = 0; i < 5; ++i)
        data.autocast_[i] = autocast_[i];
    data.health_ = base_health_;
    data.current_health_ = current_health_;
    data.strength_ = strength_;
    data.agility_ = agility_;
    data.intelligence_ = intelligence_;
    data.move_speed_ = move_speed_;
    data.class_ = class_;
    data.kills_ = total_kills_;
    data.assists_ = total_assists_;
    data.damage_done_ = total_dmg_done_;
    data.damage_taken_ = total_dmg_taken_;
    data.largest_given_ = largest_hit_done_;

    // TODO:: ITEMS
    data.base_damage_ = (class_ == encounter::hero_class::warrior ? 10 :
                         (class_ == encounter::hero_class::rogue ? 8 :
                          (class_ == encounter::hero_class::mage ? 6 : 0)));
}
void party_member::update(const encounter::hero_data& data)
{
    current_health_ = data.current_health_;
    total_kills_ += data.kills_;
    total_assists_ += data.assists_;
    total_dmg_done_ += data.damage_done_;
    total_dmg_taken_ += data.damage_taken_;
    largest_hit_done_ = (data.largest_given_ > largest_hit_done_ ? data.largest_given_ : largest_hit_done_);
    largest_hit_taken_ = (data.largest_taken_ > largest_hit_taken_ ? data.largest_taken_ : largest_hit_taken_);
    total_ff_done_ += data.friendly_fire_;
    total_battles_++;
    for (unsigned i = 0; i < 5; ++i)
        autocast_[i] = data.autocast_[i];
}
int party_member::get_current_xp() const
{
    return experience_ - xp_requirements_[level_-1];
}
int party_member::get_xp_tnl() const
{
    if (level_ < 10)
        return xp_requirements_[level_] - xp_requirements_[level_-1];
    else
        return 0;
}
int party_member::get_xp_at(unsigned level) const
{
    if (level <= 10 && level > 0)
        return xp_requirements_[level-1];
    else
        return 0;
}
int party_member::get_tnl_to(unsigned level) const
{
    if (level <= 10 && level > 0)
        return xp_requirements_[level-1] - (level > 1 ? xp_requirements_[level-2] : 0);
    else
        return 0;
}
void party_member::add_xp(int value)
{
    if (level_ > 10)
        return;
    experience_ += value;
    while (experience_ >= xp_requirements_[level_] && level_ < 10)
    {
        level_++;
        unspent_attrib_pts_ += 5;
        unspent_skill_pts_++;
        heal(0.2);
    }
}

bool party_member::spend_attrib_point(unsigned index)
{
    if (unspent_attrib_pts_ <= 0)
        return false;
    switch(static_cast<attribs::type>(index))
    {
    case attribs::intelligence:
        intelligence_++; break;
    case attribs::strength:
        strength_++;
        base_health_ = 100 + (strength_ * encounter::attribute_mod::STR_HEALTH_MOD);
        current_health_ += encounter::attribute_mod::STR_HEALTH_MOD;
        break;
    case attribs::agility:
        agility_++; break;
    default: break;
    }
    unspent_attrib_pts_--;
    return true;
}
unsigned party_member::get_attribute(unsigned index) const
{
    switch(static_cast<attribs::type>(index))
    {
    case attribs::intelligence:
        return intelligence_; break;
    case attribs::strength:
        return strength_; break;
    case attribs::agility:
        return agility_; break;
    default: return 0; break;
    }
}
bool party_member::spend_skill_point(unsigned index)
{
    if (unspent_skill_pts_ <= 0)
        return false;
    if ((index < 3 && skill_levels_[index] < 3) ||
        (index == 3 && skill_levels_[index] == 0))
    {
        skill_levels_[index]++;
        unspent_skill_pts_--;
        return true;
    }
    else
    {
        return false;
    }
}
std::string party_member::get_name(unsigned char_limit) const
{
    if (char_limit == 0 || std::string(name_+" "+surname_).size() <= char_limit)
        return std::string(name_+" "+surname_);
    else
        return name_ + " " + surname_.substr(0, 1) + ".";
}

bool party_member::can_level_skill(unsigned index) const
{
    if (unspent_skill_pts_ <= 0)
        return false;
    if ((index < 3 && skill_levels_[index] < 3) ||
        (index == 3 && skill_levels_[index] == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void party_member::set_autocast(unsigned index, bool status)
{
    utils::yth_node* class_node = resource_handler::get()->get_root_node("classes");
    switch (class_)
    {
        case encounter::hero_class::warrior: class_node = class_node->child("warrior"); break;
        case encounter::hero_class::rogue: class_node = class_node->child("rogue"); break;
        case encounter::hero_class::mage: class_node = class_node->child("mage"); break;
    }
    utils::yth_node* skill_node = resource_handler::get()->get_root_node("skills");
    skill_node = skill_node->child(class_node->child("skill", index)->value());
    if (atoi(skill_node->child("flags")->value().c_str()) & encounter::skill_flags::CAN_AUTOCAST)
        autocast_[index] = status;
}
}   // namespace overworld
}   // namespace icarus