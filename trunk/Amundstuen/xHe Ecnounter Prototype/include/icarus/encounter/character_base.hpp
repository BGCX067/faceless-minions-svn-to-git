#ifndef ICARUS_ENCOUNTER_CHARACTER_BASE_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_BASE_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "icarus/math/vector2.hpp"
#include "icarus/encounter.hpp"
#include "animate.hpp"
#include "combat_text.hpp"
#include "ui_hp_bar.hpp"

namespace icarus
{
namespace encounter
{
class grid;
namespace damage_type
{
enum type
{
    normal,
    critical,
    block,
    resist,
    overkill
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
    };
    grid* grid_ptr_;
    math::vector2f target_position_;
    math::vector2f prev_position_;
    math::vector2i vector_position_;
    math::vector2i target_vec_pos_;
    float move_speed_;
    float movement_step_;
    int health_, current_health_;
    std::string name_;
    combat_text combat_text_;
    bool is_casting_;
    ui_bar cast_bar_;
    ui_hp_bar hp_bar_;

    std::vector<path_node*> path_to_target_;
    std::vector<active_effect> effects_;

public:
    character_base(grid* grid_ptr, std::string name, std::string directory);
    character_base(grid* grid_ptr, std::string name);
    ~character_base();

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
    void update_cast_bar(float remaining, float duration);
    bool apply_damage(int damage);

    virtual void update(float delta) { combat_text_.update(delta); }
    virtual void apply_skill(const targeted_skill& skill);
    virtual void apply_effect(const applied_effect& effect);
    void update_movement();

    const std::vector<path_node*>& get_target_path() const { return path_to_target_; }
    float get_step() const { return movement_step_; }
    int get_y_depth() const { return vector_position_.y; }
    math::vector2i& get_vector_pos() { return vector_position_; }
    const math::vector2f& get_target_pos() const { return target_position_; }
    const math::vector2i& get_target_vec_pos() const { return target_vec_pos_; }
    void set_target_vec_pos(const math::vector2i& pos) { target_vec_pos_ = pos; }
    const std::string& get_name() const { return name_; }
    void set_casting(bool casting) { is_casting_ = casting; }
    bool is_casting() { return is_casting_; }
/*
    virtual void draw
    (sf::RenderTarget& target, sf::RenderStates states) const;
    //virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
*/
    //void animate();
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_BASE_HPP_
