#ifndef ICARUS_OVERWORLD_PARTY_MEMBER_HPP_
#define ICARUS_OVERWORLD_PARTY_MEMBER_HPP_

#include "icarus/encounter/data.hpp"
#include "icarus/utils.hpp"
#include "icarus/overworld/item.hpp"
#include "icarus/utils/serializable.hpp"

// very basic at the moment, probably needs some rewriting later on
namespace icarus
{
namespace overworld
{
class party_member : public utils::serializable
{
private:
    unsigned party_index_;
    std::string name_;
    std::string surname_;
    unsigned gender_;
    unsigned first_name_index_;
    unsigned surname_index_;
    int skill_levels_[4];
    bool autocast_[5];
    int xp_requirements_[10];
    item* inventory_[4];
    item* backpack_[2];
    int base_health_;
    int effective_health_;
    int current_health_;
    int agility_;
    int strength_;
    int intelligence_;
    float move_speed_;
    unsigned level_;
    int experience_;
    unsigned total_kills_;
    unsigned total_assists_;
    int total_dmg_done_;
    int total_dmg_taken_;
    int largest_hit_done_;
    int largest_hit_taken_;
    int total_ff_done_;
    unsigned total_battles_;
    unsigned unspent_skill_pts_;
    unsigned unspent_attrib_pts_;
    encounter::hero_class::type class_;
    const float level_exponent_;
public:
    party_member(unsigned index);
    party_member(unsigned index,
                 const std::string& name,
                 const std::string& surname,
                 unsigned gender,
                 unsigned name_index,
                 unsigned surname_index,
                 encounter::hero_class::type char_class,
                 unsigned level);
    virtual ~party_member();

    void generate_hero_data(encounter::hero_data& data) const;
    void update(const encounter::hero_data& data);
    int get_current_xp() const;
    int get_xp_tnl() const;
    int get_xp_at(unsigned level) const;
    int get_tnl_to(unsigned level) const;
    void add_xp(int value);
    bool spend_attrib_point(unsigned index);
    bool spend_skill_point(unsigned index);
    unsigned get_attribute(unsigned index) const;
    bool can_level_skill(unsigned index) const;

    bool destroy_item(unsigned index, bool backpack);
    bool swap_item(item* new_item, item*& old_item, unsigned index, bool backpack);
    bool internal_swap(unsigned inv_index, unsigned bp_index);
    item* get_item_at(unsigned index, bool backpack) const;
    void apply_item(item* new_item);
    void apply_all_items();

    virtual void save(std::ofstream& stream);
    virtual void load(std::ifstream& stream);
    void load_character(const utils::character& data);

    unsigned get_level() const { return level_; }
    std::string get_name(unsigned char_limit = 0) const;
    bool is_dead() const { return (current_health_ <= 0); }
    float get_current_hp() const { return float(current_health_) / effective_health_; }
    int get_current_hp_abs() const { return current_health_; }
    void set_current_hp_abs(unsigned value) { current_health_ = value; }
    void heal(float amount) { current_health_ = math::clamp(int(current_health_ * (1.0f + amount)), 0, effective_health_); }
    encounter::hero_class::type get_class() const { return class_; }
    unsigned get_total_battles() const { return total_battles_; }
    unsigned get_skill_pts() const { return unspent_skill_pts_; }
    unsigned get_attrib_pts() const { return unspent_attrib_pts_; }
    unsigned get_skill_lvl(unsigned index) const { return skill_levels_[index]; }
    bool unallocated_pts() const
    {
        return (unspent_attrib_pts_ > 0 || unspent_skill_pts_ > 0);
    }
    void set_index(unsigned index) { party_index_ = index; }
    bool get_autocast(unsigned index) const { return autocast_[index]; }
    void set_autocast(unsigned index, bool status);
    void override_kill() { current_health_ = 0; }
    unsigned get_index() const { return party_index_; }
};
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_PARTY_MEMBER_HPP_
