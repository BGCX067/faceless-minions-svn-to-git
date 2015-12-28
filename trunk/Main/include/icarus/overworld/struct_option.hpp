#include <string>
#include <vector>

namespace icarus
{
namespace overworld
{
enum type_option {ATTACK_, FLEE_, AVOID_, NEGOTIATE_};
enum type_requirement {ROGUE_, BARD_, FIGHTER_, MAGE_, PRIEST_, MERCHANT_};
enum enum_class {IRONCLAD, ALCHEMIST, QUICKSILVER};
enum enum_passive{PRIEST, MERCHANT, BARD};
enum enum_item {HEAD, BODY, HANDS, UTILITY};
enum enum_item_ability{ITEM_DAMAGE, ITEM_COLDOWN, ITEM_STRENGTH, ITEM_AGILITY, ITEM_INTELIGENCE};

struct struct_class
{
    enum_class clas_name;
    std::vector<int> skill_levels;
    std::vector<bool> feet_is_active;
    struct_class()
    {
        skill_levels.push_back(0);
        skill_levels.push_back(0);
        skill_levels.push_back(0);
        skill_levels.push_back(0);
        feet_is_active.push_back(false); //Level  2
        feet_is_active.push_back(false); //Level  4
        feet_is_active.push_back(false); //Level  6
        feet_is_active.push_back(false); //Level  8
        feet_is_active.push_back(false); //Level 10
    }
};
struct struct_item
{
    std::string item_name;
    int item_value;
    enum_item item_slot;
    std::vector<float> item_ability;
    std::string item_image;
    struct_item()
    {
        item_ability.push_back(0); //Damage
        item_ability.push_back(0); //coldown_multiplyer
        item_ability.push_back(0); //Strength    enchantment
        item_ability.push_back(0); //Agility     enchantment
        item_ability.push_back(0); //Inteligence enchantment
    }
};
struct struct_player
{
    std::string name;
    struct_class player_class;
    enum_passive player_passive;
    std::vector<int> player_stats;
    std::vector<struct_item> item_slots;
    int experience_points;

    struct_player()
    {
        player_stats->push_back(0);//Strength
        player_stats->push_back(0);//Agility
        player_stats->push_back(0);//Inteligence
        struct_item starter;
        item_slots.push_back(starter);//head
        item_slots.push_back(starter);//body
        item_slots.push_back(starter);//hands
        item_slots.push_back(starter);//utility
    }
};

struct struct_party
{
    std::vector<struct_player> party_members;
    std::vector<struct_item> party_inventory;
    int party_coins;
};

} // namespace overworld
} // namespace icarus
