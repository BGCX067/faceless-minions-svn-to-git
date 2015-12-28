#ifndef STATISTICS_H
#define STATISTICS_H
namespace icarus
{
namespace overworld
{
enum state {PLAYER_, VISITABLE_, OCUPIED_, UNREACHABLE_, VISIBLE_};

struct statistics
{

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
    }
};
} // namespace overworld
} // namespace icarus
#endif // STATISTICS_H
