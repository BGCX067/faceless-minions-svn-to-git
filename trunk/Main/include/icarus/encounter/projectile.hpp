#ifndef ICARUS_ENCOUNTER_PROJECTILE_HPP_
#define ICARUS_ENCOUNTER_PROJECTILE_HPP_

#include <vector>
#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter.hpp"
#include "icarus/utils/yth_handler.hpp"
#include "icarus/ui/sprite_sheet.hpp"
#include "icarus/encounter/grid_tile.hpp"

namespace icarus
{
namespace encounter
{
class projectile : public animate
{
private:
    targeted_skill subskill_;
    float speed_, v_ratio_, h_ratio_;
    float angle_;
    float interval_timer_;
    float current_interval_;
    math::vector2f projectile_offset_;
    icarus::ui::sprite_sheet shadow_;
    character_base* caster_;
    std::vector<character_base*> missed_targets_;

    bool is_railgun_;
    bool reverse_effects_;
    std::vector<math::vector2i> affected_tiles_;
    grid_tile* target_;

public:
    projectile(float speed, utils::yth_node* data_node,
               bool railgun = false);
    virtual ~projectile();
    void set_skill(const targeted_skill& skill);
    void set_speed_ratios(float angle);
    void update(float delta);
    math::vector2f get_position() const;
    void set_initial_pos(const math::vector2f& pos);
    math::vector2f get_projectile_pos() const;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void set_caster(character_base* caster) { caster_ = caster; }
    character_base* get_caster() const { return caster_; }
    targeted_skill& get_subskill() { return subskill_; }
    void add_miss(character_base* target) { missed_targets_.push_back(target); }
    bool has_missed_target(character_base* target) const
    {
        for (unsigned i = 0; i < missed_targets_.size(); ++i)
            if (target == missed_targets_[i])
                return true;
        return false;
    }

    bool is_railgun() const { return is_railgun_; }
    bool tile_already_affected(const math::vector2i& vpos) const
    {
        for (unsigned i = 0; i < affected_tiles_.size(); ++i)
            if (vpos == affected_tiles_[i])
                return true;
        return false;
    }
    void add_affected_tile(const math::vector2i& vpos)
    {
        affected_tiles_.push_back(vpos);
    }
    void setup_interval(float interval)
    {
        current_interval_ = interval_timer_ = interval;
    }
    bool update_interval(double delta)
    {
        if ((current_interval_ -= delta) < 0.0f)
        {
            current_interval_ = interval_timer_;
            return true;
        }
        return false;
    }
    void set_reverse(bool status) { reverse_effects_ = status; }
    bool get_reverse() const { return reverse_effects_; }
    grid_tile* get_target() const { return target_; }
    void set_target(grid_tile* target) { target_ = target; }
};
}   // namespace encounter
} // namespace icarus
#endif // ICARUS_ENCOUNTER_PROJECTILE_HPP_
