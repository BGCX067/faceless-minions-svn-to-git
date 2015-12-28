#ifndef ICARUS_ENCOUNTER_ANIMATE_HPP_
#define ICARUS_ENCOUNTER_ANIMATE_HPP_

#include <SFML/Graphics.hpp>
#include <iostream>
#include "icarus/state_manager.hpp"
#include "icarus/math/vector2.hpp"
#include "icarus/config.hpp"
#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace anim_state    // renamed this for clarity
{
enum value
{
    idle,
    walk,
    basic_melee,
    basic_ranged,
    basic_spell
};
} //Namespace animation_specss

struct animation                // small struct to contain animation data
{
    int start_x_;          // the x and y positions in the spritesheet
    int start_y_;          // where the animation starts
    int frame_width_;      //  the width and height of the animation frames
    int frame_height_;
    int origin_x_;         // the x and y coordinates of the origin point
    int origin_y_;
    int frame_count_;      // number of frames in this animation
};

class animate: public sf::Drawable
{
    protected:
//        unsigned sprite_width_;
//        unsigned sprite_height_;
//        unsigned sheet_width_;
//        unsigned sheet_height_;
//        unsigned sheet_columns_;
//        unsigned sheet_row_;

        // var to hold the active animation, duh
        animation active_animation_;
        int current_frame_;

        // an unordered map, using the animation states as key,
        // to contain the texture coordinates for different animations
        std::unordered_map<std::string, animation> anim_map_;

        // switching to strings for now
        std::string animation_state_;
        std::string prev_anim_state_;
        bool one_shot_;

        double time_elapsed_;
        unsigned target_framerate_;
        static int const standard_framerate_ = 30;

        math::vector2i char_grid_pos_;

        //std::string sprite_directory_; // do we need this? I don't think we do.
        sf::Sprite* character_sprite_;

    public:
        animate();
        // this shouldn't be needed anymore
        //animate(const std::string& sprite_dir);

        virtual ~animate();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    // adding a yth_node pointer argument, will point to the relevant entry
    // in the spritesheets.yth file, and a string reference to load the appropriate
    // textures
    void load_sprite(utils::yth_node* sprite_node,
                     const std::string& sheet_path);
    void update_anim();
    virtual void update_pos(const math::vector2f& sprite_updated,
                            const math::vector2i& grid_updated);
    math::vector2f get_world_pos();
    void set_world_pos(const math::vector2f& new_pos); // dirty hack for cursor
    void orient_sprite(bool positive_axis);
    void set_animation_state(const std::string& new_state, bool one_shot = false);
    bool contains(const math::vector2f& pos) const { return character_sprite_->getGlobalBounds().contains(pos); }
};
}   // namespace encounter
}   // namespace icarus



#endif // ICARUS_ENCOUNTER_ANIMATE_HPP_
