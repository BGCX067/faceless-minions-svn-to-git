#ifndef ICARUS_OVERWORLD_PARTY_MEMBER_HPP_
#define ICARUS_OVERWORLD_PARTY_MEMBER_HPP_

#include "icarus/encounter/data.hpp"
#include "icarus/utils.hpp"

// very basic at the moment, probably needs some rewriting later on
namespace icarus
{
namespace overworld
{
class party_member
{
private:
    unsigned party_index_;
    std::string name_;
    std::string surname_;
    int skill_levels_[4];
    int xp_requirements_[10];
    int base_health_;
    float current_health_;
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
    party_member(unsigned index,
                 const std::string& name,
                 const std::string& surname,
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

    unsigned get_level() const { return level_; }
    std::string get_name(unsigned char_limit = 0) const;
    bool is_dead() const { return (current_health_ <= 0.0f); }
    float get_current_hp() const { return current_health_; }
    void heal(float amount) { current_health_ = math::clamp(current_health_ + amount, 0.0f, 1.0f); }
    encounter::hero_class::type get_class() const { return class_; }
    unsigned get_total_battles() const { return total_battles_; }
    unsigned get_skill_pts() const { return unspent_skill_pts_; }
    unsigned get_attrib_pts() const { return unspent_attrib_pts_; }
    unsigned get_skill_lvl(unsigned index) const { return skill_levels_[index]; }
};
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_PARTY_MEMBER_HPP_
