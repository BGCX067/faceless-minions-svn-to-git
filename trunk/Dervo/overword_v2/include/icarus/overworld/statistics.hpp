#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <string>

namespace icarus
{
namespace overworld
{
enum state {PLAYER_, VISITABLE_, OCUPIED_, UNREACHABLE_, VISIBLE_};
enum nation {RED_, BLUE_, GREEN_, SPAWN_, NEUTRAL_};

//enum siege {PREBATTLE_, BATTLE_};

struct statistics
{
public:
    // Hex_sprite
    unsigned grid_width_;
    unsigned grid_height_;
    unsigned pixel_width_;
    unsigned pixel_height_;
    std::string url_hex_tiles_;
    unsigned hex_offset_;
    unsigned num_fractions_;

    unsigned  hex_sprite_margin;

    //character
    std::string url_character_sprite_;
    unsigned character_pixel_width_;
    unsigned character_pixel_height_;

    //battle
    float army_size;
    float battle_modifier;
    std::vector <float> army_size_per_nation;

    statistics()
    {
        grid_width_ = 7;
        grid_height_ = 7;

        pixel_width_ = 180;
        pixel_height_ = 180;

        url_hex_tiles_ = "hex_mal_v5.png";

        hex_offset_ = 99;

        num_fractions_ = 3+1;

        url_character_sprite_ = "sprite_char.png";

        character_pixel_height_ = 50;
        character_pixel_width_ = 50;

        hex_sprite_margin = 81;

        army_size = 0.5f;
        battle_modifier = 0.5f;
        army_size_per_nation.push_back(0.0f);
        army_size_per_nation.push_back(0.0f);
        army_size_per_nation.push_back(0.0f);
    }
};
} // namespace overworld
} // namespace icarus
#endif // STATISTICS_H
