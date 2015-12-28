#ifndef ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_
#define ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_

#include "icarus/encounter/character_base.hpp"
#include "icarus/encounter/composite_skill.hpp"
#include "icarus/encounter/data.hpp"
#include "icarus/encounter/ui/portrait.hpp"

namespace icarus
{
namespace encounter
{
class character_hero : public character_base
{
private:
    // stats
    int strength_, intelligence_, agility_;
    int str_mod_, int_mod_, agi_mod_;
    hero_class::type class_;
    int modified_hp_;
    std::string surname_;

    composite_skill active_skills_[5];
    ui::portrait* portrait_;
    struct queued_skill
    {
        int index_;
        math::vector2i target_;
    };
    queued_skill queued_skill_;
    character_base* favored_target_;

    void setup_skills(const std::string& char_class, const unsigned (&levels)[4],
                      const bool (&autocast)[5]);
    void setup_portrait(unsigned index, utils::yth_node* class_node);
    virtual void update_hp_bar(float fraction);

public:
    character_hero(grid* grid_ptr, std::string name);
    character_hero(grid* grid_ptr,
                   unsigned sprite_height,
                   unsigned sprite_width,
                   unsigned sheet_height,
                   unsigned sheet_width,
                   std::string name);
    virtual ~character_hero();

    virtual void update(double delta);
    void setup_from_data(const hero_data& data, unsigned index);
    void get_data(hero_data& data);
    virtual void draw(sf::RenderTarget& target,
                      sf::RenderStates states) const;
    virtual void update_cast_bar(float remaining, float duration);
    const sf::Texture* get_portrait_ptr();
    virtual std::string get_name(unsigned char_limit = 0) const;

    composite_skill& get_skill(int index) { return active_skills_[index]; }
    void set_selected(bool state) { portrait_->set_selected(state); }
    void set_queued_skill(int index, const math::vector2i& target)
    {
        queued_skill_.index_ = index;
        queued_skill_.target_ = target;
    }
    const queued_skill& get_queued_skill() const { return queued_skill_; }
    void set_favored_target(character_base* new_target) { favored_target_ = new_target; }
    character_base* get_favored_target() const { return favored_target_; }
    bool portrait_contains(const math::vector2f& pos) const { return portrait_->contains(pos); }
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_CHARACTER_HERO_HPP_
