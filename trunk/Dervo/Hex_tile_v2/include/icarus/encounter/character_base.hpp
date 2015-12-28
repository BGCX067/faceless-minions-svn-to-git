#ifndef ICARUS_ENCOUNTER_CHARACTER_BASE_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_BASE_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <set>

#include "icarus/math/vector2.hpp"
#include "icarus/encounter.hpp"
#include "icarus/encounter/animate.hpp"
#include "icarus/encounter/combat_text.hpp"
#include "icarus/encounter/ui/hp_bar.hpp"

namespace icarus
{
namespace encounter
{
class grid;
class grid_tile;
namespace damage_type
{
enum type
{
    normal,
    critical,
    block,
    resist,
    dodge,
    critical_block,
    critical_resist,
    critical_dodge,
    overkill,
    stun
};
} // namespace damage_type
class character_base: public animate
{
private:

protected:
    struct active_effect
    {
        applied_effect effect;
        int ticks_remaining;
        float tick_step;
        float time_left;
        char flags;
    };
    grid* grid_ptr_;
    math::vector2f target_position_;
    math::vector2f prev_position_;
    math::vector2i vector_position_;
    math::vector2i target_vec_pos_;
    float move_speed_;
    float movement_step_;
    bool is_moving_;
    int health_, current_health_;
    std::string name_;
    combat_text combat_text_;
    bool is_casting_;
    ui::bar cast_bar_;
    ui::hp_bar hp_bar_;
    float path_timer_;
    grid_tile* retry_target_;
    bool is_ai_;
    bool is_obstacle_;
    bool is_dead_;
    unsigned weapon_damage_;

    unsigned dodge_chance_;
    unsigned resist_chance_;
    float resist_amount_;
    unsigned block_chance_;
    float block_amount_;
    unsigned critical_chance_;
    float critical_mod_;
    float speed_mod_;
    float phys_damage_mod_;
    float magic_damage_mod_;

    bool flash_;
    utils::countdown flash_timer_;
    unsigned flash_count_;
    bool render_;
    bool show_health_bar_;
    bool show_cast_bar_;
    int incapacitated_;
    bool debug_;
    sf::VertexArray debug_line_;

    unsigned kills_, assists_;
    int damage_dealt_, damage_received_,
    largest_taken_, largest_given_, friendly_fire_;
    std::set<character_base*> dmg_contributors_;

    std::vector<path_node*> path_to_target_;
    std::vector<active_effect> effects_;
    std::vector<stat_mod> mods_;

    virtual void update_hp_bar(float fraction);
    virtual void apply_stat_mods(const applied_effect& effect);

public:
    character_base(grid* grid_ptr, const std::string& name);
    virtual ~character_base();

    void set_target_path(std::vector<path_node*> new_path);
    void set_target_pos(const math::vector2f& new_pos);
    sf::FloatRect get_bounds(bool use_global = true) const;
    void move_one_step();
    void pop_path();
    virtual void update_pos(const math::vector2f& sprite_updated,
                            const math::vector2i& grid_updated);
    virtual void draw(sf::RenderTarget& target,
                      sf::RenderStates states) const;
    virtual void print_damage(int damage,
                              damage_type::type dmg_type,
                              int alt_damage = 0); // alt damage = block/resist/overkill amount
    virtual void update_cast_bar(float remaining, float duration);
    virtual bool apply_damage(int damage, uint8_t flags, character_base* source);

    virtual void update(double delta) { combat_text_.update(delta); }
    virtual void apply_skill(const targeted_skill& skill);
    virtual void apply_effect(const applied_effect& effect);
    virtual void remove_effect(const applied_effect& effect);
    void update_movement();
    void death();
    void clear_path();
    void print_dodge(bool critical);

    const std::vector<path_node*>& get_target_path() const { return path_to_target_; }
    float get_step() const { return movement_step_; }
    int get_y_depth() const { return vector_position_.y; }
    math::vector2i& get_vector_pos() { return vector_position_; }
    const math::vector2f& get_target_pos() const { return target_position_; }
    const math::vector2i& get_target_vec_pos() const { return target_vec_pos_; }
    void set_target_vec_pos(const math::vector2i& pos) { target_vec_pos_ = pos; }
    virtual const std::string& get_name() const { return name_; }
    void set_casting(bool casting) { is_casting_ = casting; }
    bool is_casting() const { return is_casting_; }
    bool is_ai() const { return is_ai_; }
    bool is_obstacle() const { return is_obstacle_; }
    bool is_dead() const { return is_dead_; }
    virtual float get_current_health() const { return static_cast<float>(current_health_) / health_; }
    unsigned get_crit_chance() const { return critical_chance_; }
    float get_crit_mod() const { return critical_mod_; }
    float get_physical_mod() const { return phys_damage_mod_; }
    float get_magical_mod() const { return magic_damage_mod_; }
    unsigned get_dodge() const { return dodge_chance_; }
    bool is_moving() { return is_moving_; }
    void set_cast_bar_enabled(bool enabled) { show_cast_bar_ = enabled; }
    void set_health_bar_enabled(bool enabled) { show_health_bar_ = enabled; }
    void add_kill() { kills_++; }
    void add_assist() { assists_++; }
    void add_ff_dmg(int value) { friendly_fire_ += value; }
    bool is_incapacitated() const { return incapacitated_; }
    unsigned get_weapon_dmg() const { return weapon_damage_; }
    void set_show_castbar(bool show) { show_cast_bar_ = show; }
    void add_dmg_done(int value)
    {
        damage_dealt_ += value;
        if (value > largest_given_)
            largest_given_ = value;
    }
    void add_dmg_taken(int value)
    {
        damage_received_ += value;
        if (value > largest_taken_)
            largest_taken_ = value;
    }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_BASE_HPP_
