#include <cstdlib>
#include <cmath>
#include <cstring>

#include "icarus/overworld/party_member.hpp"
#include "icarus/math.hpp"
#include "icarus/encounter.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/item.hpp"

namespace icarus
{
namespace overworld
{
party_member::party_member(unsigned index)
:
    party_index_(index),
    name_(""),
    surname_(""),
    gender_(0),
    first_name_index_(0),
    surname_index_(0),
    base_health_(0),
    effective_health_(0),
    current_health_(0),
    agility_(0),
    strength_(0),
    intelligence_(0),
    move_speed_(0.0f),
    level_(0),
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
    class_(encounter::hero_class::warrior),
    level_exponent_(267.07f)
{
}
party_member::party_member(unsigned index,
                           const std::string& name,
                           const std::string& surname,
                           unsigned gender,
                           unsigned name_index,
                           unsigned surname_index,
                           encounter::hero_class::type char_class,
                           unsigned level)
:
    party_index_(index),
    name_(name),
    surname_(surname),
    gender_(gender),
    first_name_index_(name_index),
    surname_index_(surname_index),
    base_health_(0),
    effective_health_(0),
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
    effective_health_ = base_health_;
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

    inventory_[0] = NULL;
    inventory_[1] = NULL;
    inventory_[2] = NULL;
    inventory_[3] = NULL;
    backpack_[0] = NULL;
    backpack_[1] = NULL;

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
    data.health_ = effective_health_;
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

    data.damage_mod_ = 0;
    data.crit_chance_ = 0;
    data.dodge_chance_  = 0;
    data.block_chance_ = 0;
    data.resist_chance_ = 0;
    data.crit_mult_ = 0;
    data.block_mult_ = 0;
    data.resist_mult_ = 0;

    // TODO:: ITEMS
    data.base_max_damage_ = (class_ == encounter::hero_class::warrior ? 12 :
                         (class_ == encounter::hero_class::rogue ? 6 :
                          (class_ == encounter::hero_class::mage ? 4 : 0)));
    data.base_min_damage_ = (class_ == encounter::hero_class::warrior ? 8 :
                         (class_ == encounter::hero_class::rogue ? 5 :
                          (class_ == encounter::hero_class::mage ? 3 : 0)));

    for (unsigned i = 0; i < 4; ++i)
    {
        if (inventory_[i] == NULL)
            continue;
        data.base_max_damage_ += inventory_[i]->get_damage_max() + inventory_[i]->get_damage();
        data.base_min_damage_ += inventory_[i]->get_damage_min() + inventory_[i]->get_damage();
        data.damage_mod_ += inventory_[i]->get_damage_mod();
        data.crit_chance_ += inventory_[i]->get_crit_chance();
        data.dodge_chance_ += inventory_[i]->get_dodge_chance();
        data.block_chance_ += inventory_[i]->get_block_chance();
        data.resist_chance_ += inventory_[i]->get_resist_chance();
        data.crit_mult_ += inventory_[i]->get_crit_mult();
        data.block_mult_ += inventory_[i]->get_block_mult();
        data.resist_mult_ += inventory_[i]->get_resist_mult();
        data.strength_ += inventory_[i]->get_str_bonus();
        data.intelligence_ += inventory_[i]->get_int_bonus();
        data.agility_ += inventory_[i]->get_agi_bonus();
    }
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
    {
        strength_++;
        float hp_ratio = float(current_health_)/float(effective_health_);
        base_health_ += encounter::attribute_mod::STR_HEALTH_MOD;
        effective_health_ += encounter::attribute_mod::STR_HEALTH_MOD;
        current_health_ = effective_health_ * hp_ratio;
        break;
    }
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
        {
        unsigned t_int = intelligence_;
        for (unsigned i = 0; i < 4; ++i)
        {
            if (inventory_[i] == NULL)
                continue;
            t_int += inventory_[i]->get_int_bonus();
        }
        return t_int;
        break;
        }
    case attribs::strength:
        {
        unsigned t_str = strength_;
        for (unsigned i = 0; i < 4; ++i)
        {
            if (inventory_[i] == NULL)
                continue;
            t_str += inventory_[i]->get_str_bonus();
        }
        return t_str;
        break;
        }
    case attribs::agility:
        {
        unsigned t_agi = agility_;
        for (unsigned i = 0; i < 4; ++i)
        {
            if (inventory_[i] == NULL)
                continue;
            t_agi += inventory_[i]->get_agi_bonus();
        }
        return t_agi;
        break;
        }
    default: return 0; break;
    }
}
bool party_member::spend_skill_point(unsigned index)
{
    if (unspent_skill_pts_ <= 0)
        return false;
    if ((index < 3 && skill_levels_[index] < 3 && (level_ == 1 || skill_levels_[index] < (int)level_ - 1)) ||
            (index == 3 && skill_levels_[index] == 0 && level_ > 5))
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
    return ((index < 3 && skill_levels_[index] < 3 && (level_ == 1 || skill_levels_[index] < (int)level_ - 1)) ||
            (index == 3 && skill_levels_[index] == 0 && level_ > 5));
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

bool party_member::destroy_item(unsigned index, bool backpack)
{
    if (backpack && index < 2 && backpack_[index] != NULL)
    {
        delete backpack_[index];
        backpack_[index] = NULL;
        return true;
    }
    else if (index < 4 && inventory_[index] != NULL)
    {
        delete inventory_[index];
        inventory_[index] = NULL;
        return true;
    }
    else
    {
        return false;
    }
}
bool party_member::swap_item(item* new_item, item*& old_item, unsigned index, bool backpack)
{
    if (backpack && index < 2)
    {
        old_item = backpack_[index];
        backpack_[index] = new_item;
        return true;
    }
    else if (index < 4)
    {
        if (new_item != NULL && static_cast<unsigned>(new_item->get_class()) != index)
            return false;
        old_item = inventory_[index];
        inventory_[index] = new_item;
        return true;
    }
    return false;
}
bool party_member::internal_swap(unsigned inv_index, unsigned bp_index)
{
    if (backpack_[bp_index] != NULL && static_cast<unsigned>(backpack_[bp_index]->get_class()) != inv_index)
        return false;
    item* temp = backpack_[bp_index];
    backpack_[bp_index] = inventory_[inv_index];
    inventory_[inv_index] = temp;
    return true;
}
item* party_member::get_item_at(unsigned index, bool backpack) const
{
    if (backpack && index < 2)
        return backpack_[index];
    else if (index < 4)
        return inventory_[index];
    else
        return NULL;
}
void party_member::apply_item(item* new_item)
{
    if (new_item == NULL)
        return;
    float hp_ratio = float(current_health_)/float(effective_health_);
    effective_health_ += new_item->get_str_bonus() * encounter::attribute_mod::STR_HEALTH_MOD;
    current_health_ = effective_health_ * hp_ratio;
}
void party_member::apply_all_items()
{
    float hp_ratio = float(current_health_) / float(effective_health_);
    effective_health_ = base_health_;
    current_health_ = effective_health_ * hp_ratio;
    for (unsigned i = 0; i < 4; ++i)
    {
        if (inventory_[i] != NULL)
            apply_item(inventory_[i]);
    }
}

void party_member::save(std::ofstream& stream)
{
    utils::character member;
    member.active_ = 1;
    member.gender_ = gender_;
    member.first_name_ = first_name_index_;
    member.surname_ = surname_index_;
    member.class_ = unsigned(class_);
    member.strength_ = strength_;
    member.intelligence_ = intelligence_;
    member.agility_ = agility_;
    member.experience_ = experience_;
    member.current_hp_ = current_health_;

    member.skill0_ = skill_levels_[0];
    member.skill1_ = skill_levels_[1];
    member.skill2_ = skill_levels_[2];
    member.skill3_ = skill_levels_[3];

    member.autocast_ = member.autocast_ | (autocast_[0] ? 0x1 : 0x0);
    member.autocast_ = member.autocast_ | (autocast_[1] ? 0x2 : 0x0);
    member.autocast_ = member.autocast_ | (autocast_[2] ? 0x4 : 0x0);
    member.autocast_ = member.autocast_ | (autocast_[3] ? 0x8 : 0x0);

    member.battle_count_ = total_battles_;
    member.damage_done_ = total_dmg_done_;
    member.damage_taken_ = total_dmg_taken_;
    member.kills_ = total_kills_;
    member.assists_ = total_assists_;
    stream.write(member.bytes, sizeof(member.bytes));

    for (unsigned i = 0; i < 4; ++i)
    {
        if (inventory_[i] != NULL)
        {
            inventory_[i]->save(stream);
        }
        else
        {
            utils::item item;
            std::memset(item.bytes, 0, sizeof(item.bytes));
            item.active_ = 0;
            stream.write(item.bytes, sizeof(item.bytes));
        }
    }
    for (unsigned i = 0; i < 2; ++i)
    {
        if (backpack_[i] != NULL)
        {
            backpack_[i]->save(stream);
        }
        else
        {
            utils::item item;
            std::memset(item.bytes, 0, sizeof(item.bytes));
            item.active_ = 0;
            stream.write(item.bytes, sizeof(item.bytes));
        }
    }
}
void party_member::load(std::ifstream& stream)
{

}
void party_member::load_character(const utils::character& data)
{
    utils::yth_node* name_node = resource_handler::get()->get_root_node("hero_names");
    name_ = name_node->child(unsigned(data.gender_))->child("name", unsigned(data.first_name_))->value();
    surname_ = name_node->child("surnames")->child("name", data.surname_)->value();

    base_health_ = 100;
    effective_health_ = base_health_;
    class_ = encounter::hero_class::type(data.class_);

    for (unsigned i = 0; i < 10; ++i)
    {
        xp_requirements_[i] = ceil(level_exponent_ * pow(1.5f, i) - level_exponent_);
    }
    experience_ = unsigned(data.experience_);
    while (experience_ >= xp_requirements_[level_] && level_ < 10)
    {
        level_++;
    }

    // TODO:: fix proper generation for other stats
    move_speed_ = 250.0f;

    autocast_[0] = (data.autocast_ & 0x1);
    autocast_[1] = (data.autocast_ & 0x2);
    autocast_[2] = (data.autocast_ & 0x4);
    autocast_[3] = (data.autocast_ & 0x8);
    autocast_[4] = false;

    skill_levels_[0] = unsigned(data.skill0_);
    skill_levels_[1] = unsigned(data.skill1_);
    skill_levels_[2] = unsigned(data.skill2_);
    skill_levels_[3] = unsigned(data.skill3_);

    inventory_[0] = NULL;
    inventory_[1] = NULL;
    inventory_[2] = NULL;
    inventory_[3] = NULL;
    backpack_[0] = NULL;
    backpack_[1] = NULL;

    strength_ = unsigned(data.strength_);
    intelligence_ = unsigned(data.intelligence_);
    agility_ = unsigned(data.agility_);

    unspent_attrib_pts_ = (level_ * 5) - (strength_ + intelligence_ + agility_);
    unspent_skill_pts_ = level_ - (skill_levels_[0] + skill_levels_[1] + skill_levels_[2] + skill_levels_[3]);

    base_health_ += strength_ * encounter::attribute_mod::STR_HEALTH_MOD;
    effective_health_ += strength_ * encounter::attribute_mod::STR_HEALTH_MOD;

    total_kills_ = unsigned(data.kills_);
    total_assists_ = unsigned(data.assists_);
    total_dmg_done_ = unsigned(data.damage_done_);
    total_dmg_taken_ = unsigned(data.damage_taken_);
    total_battles_ = unsigned(data.battle_count_);
}
}   // namespace overworld
}   // namespace icarus
