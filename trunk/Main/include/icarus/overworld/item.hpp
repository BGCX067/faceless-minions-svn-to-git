#ifndef ICARUS_OVERWORLD_ITEM_HPP_
#define ICARUS_OVERWORLD_ITEM_HPP_

#include <string>
#include <vector>
#include "icarus/utils.hpp"
#include "icarus/utils/serializable.hpp"

namespace icarus
{
namespace overworld
{
namespace item_class
{
enum value
{
    weapon,
    armor,
    misc,
    hat
};
}
class item : public utils::serializable
{
private:
    std::string name_;
    std::string display_name_;
    std::string flavortext_;
    std::string icon_name_;
    item_class::value item_class_;
    unsigned base_index_;
    int prefix_index_;
    int postfix_index_;
    bool is_unique_;
    // offense
    unsigned damage_;
    unsigned damage_min_;
    unsigned damage_max_;
    float damage_mod_;
    unsigned crit_chance_;
    float crit_mult_;

    // defense
    unsigned dodge_chance_;
    unsigned block_chance_;
    float block_mult_;
    unsigned resist_chance_;
    float resist_mult_;
    unsigned health_bonus_;
    unsigned health_regen_;

    // stats
    unsigned strength_bonus_;
    unsigned intelligence_bonus_;
    unsigned agility_bonus_;
    utils::yth_node* tt_data_node_;

    void apply_stats(utils::yth_node* item_node);
    void reset_stats();
    void setup_tt_data();

public:
    item();
    virtual ~item();

    void generate_random(unsigned level);
    void get_random_unique();
    void get_unique(const std::string& name);

    virtual void save(std::ofstream& stream);
    virtual void load(std::ifstream& stream);
    void load_item(const utils::item& data);

    const std::string& get_name() const { return name_; }
    const std::string& get_displayname() const { return display_name_; }
    const std::string& get_flavortext() const { return flavortext_; }
    const std::string& get_icon_name() const { return icon_name_; }
    item_class::value get_class() const { return item_class_; }
    unsigned get_damage() const { return damage_; }
    unsigned get_damage_min() const { return damage_min_; }
    unsigned get_damage_max() const { return damage_max_; }
    float get_damage_mod() const { return damage_mod_; }
    unsigned get_crit_chance() const { return crit_chance_; }
    float get_crit_mult() const { return crit_mult_; }
    unsigned get_dodge_chance() const { return dodge_chance_; }
    unsigned get_block_chance() const { return block_chance_; }
    float get_block_mult() const { return block_mult_; }
    unsigned get_resist_chance() const { return resist_chance_; }
    float get_resist_mult() const { return resist_mult_; }
    unsigned get_health_bonus() const { return health_bonus_; }
    unsigned get_health_regen() const { return health_regen_; }
    unsigned get_str_bonus() const { return strength_bonus_; }
    unsigned get_int_bonus() const { return intelligence_bonus_; }
    unsigned get_agi_bonus() const { return agility_bonus_; }
    utils::yth_node* get_tt_data_node() { return tt_data_node_; }
};
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_ITEM_HPP_
