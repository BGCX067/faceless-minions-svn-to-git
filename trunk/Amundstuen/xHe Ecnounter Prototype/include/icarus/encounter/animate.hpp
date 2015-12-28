#ifndef ICARUS_ENCOUNTER_ANIMATE_HPP_
#define ICARUS_ENCOUNTER_ANIMATE_HPP_

#include <SFML/Graphics.hpp>
#include <iostream>
#include "icarus/state_manager.hpp"
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace anim_specs
{
    enum anim_state
    {
        idle,
        walk,
        basic_melee,
        basic_ranged,
        basic_spell
    };
} //Namespace animation_specss
namespace encounter
{
class animate: public sf::Drawable
{
    protected:
        unsigned sprite_width_;
        unsigned sprite_height_;
        unsigned sheet_width_;
        unsigned sheet_height_;
        unsigned sheet_columns_;
        unsigned sheet_row_;
        anim_specs::anim_state animation_state_;

        float target_framerate_;

        math::vector2i char_grid_pos_;

        std::string sprite_directory_;
        sf::Sprite* character_sprite_;

    public:
        animate();
        animate(std::string sprite_dir);

        virtual ~animate();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void load_sprite();
    void update_anim();
    virtual void update_pos(const math::vector2f& sprite_updated,
                            const math::vector2i& grid_updated);
    math::vector2f get_world_pos();
    void orient_sprite(bool positive_axis);
    void set_animation_state(anim_specs::anim_state
                             new_state);

};
}   // namespace encounter
}   // namespace icarus



#endif // ICARUS_ENCOUNTER_ANIMATE_HPP_
