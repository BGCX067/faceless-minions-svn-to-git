#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <string>

namespace icarus
{
namespace overworld
{
enum state {PLAYER_, VISITABLE_, OCUPIED_, UNREACHABLE_, VISIBLE_};
enum nation {RED_, GREEN_, BLUE_, SPAWN_, NEUTRAL_};
enum nature {GRASS_, FORREST_, HILL_, LAKE_};
enum placement {TAVERN_, BATTLEZONE_, CARAVAN_, CITIES_, RUINDS_, DUNGEON_};

enum sheet_nature{  GRASS_1,GRASS_2,GRASS_3,GRASS_4,GRASS_5,
                    GRASS_6,GRASS_7,GRASS_8,GRASS_9,

                    HILL_1, HILL_2, HILL_3, HILL_4, HILL_5,
                    HILL_6, HILL_7, HILL_8, HILL_9,

                    LAKE_1, LAKE_2, LAKE_3, LAKE_4, LAKE_5,
                    LAKE_6, LAKE_7, LAKE_8, LAKE_9,

                    ROCK_1, ROCK_2, ROCK_3, ROCK_4, ROCK_5,
                    ROCK_6, ROCK_7, ROCK_8, ROCK_9};

enum sheet_border{  B_RED_1, B_RED_2, B_RED_3, B_RED_4, B_RED_5, B_RED_6,
                    B_GREEN_1,B_GREEN_2,B_GREEN_3,B_GREEN_4,B_GREEN_5,B_GREEN_6,
                    B_BLUE_1, B_BLUE_2, B_BLUE_3, B_BLUE_4, B_BLUE_5, B_BLUE_6,
                    BLANK};

enum sheet_fraction{RED_PLANE, RED_CITY_1, RED_CITY_2, RED_CITY_3,          SF_U_1, SF_U_2, SF_U_3, SF_U_4, SF_U_5,
                    GREEN_PLANE, GREEN_CITY_1, GREEN_CITY_2, GREEN_CITY_3,  SF_U_6, SF_U_7, SF_U_8, SF_U_9, SF_U_10,
                    BLUE_PLANE, BLUE_CITY_1, BLUE_CITY_2, BLUECITY_3,       SF_U_11,SF_U_12,SF_U_13,SF_U_14,SF_U_15,
                    SPAWN_PLANE, SPAWN_POINT,               SF_U_16,SF_U_17,SF_U_18,SF_U_19,SF_U_20,SF_U_21,SF_U_22,
                    NEUTRAL_PLANE};

enum sheet_structures{  MOUNTAIN_1, MOUNTAIN_2, MOUNTAIN_3,
                        FORREST_1, FORREST_2, FORREST_3,
                        UNUSED_1, UNUSED_2, UNUSED_3,

                        TAVERN_1, TAVERN_2, TAVERN_3,
                        DUNGEON_1, DUNGEON_2, DUNGEON_3,
                        RUIN_1, RUIN_2, RUIN_3,

                        CARAVAN_1, CARAVAN_2, CARAVAN_3};

enum type_texture {BASE_IMAGE, BORDER_0, BORDER_1, BORDER_2, BORDER_3, BORDER_4, BORDER_5, STRUCTURE };
//enum siege {PREBATTLE_, BATTLE_};

struct statistics
{
public:
    std::vector<unsigned> finished_nature;
    std::vector<unsigned> finished_border;
    std::vector<unsigned> finished_factions;
    std::vector<unsigned> finished_structures;
    // Hex_sprite
    unsigned grid_width_;
    unsigned grid_height_;
    unsigned pixel_width_;
    unsigned pixel_height_;
    unsigned hex_offset_;
    unsigned num_fractions_;

    std::string url_nature_;
    std::string url_border_;
    std::string url_factions_;
    std::string url_structures_;

    unsigned  hex_sprite_margin;

    //character
    std::string url_character_sprite_;
    unsigned character_pixel_width_;
    unsigned character_pixel_height_;

    //battle
    float army_size;
    float battle_modifier;
    std::vector <float> army_size_per_nation;

    //screen
    float screen_width_;
    float screen_height_;

    float scenter_x_;
    float scenter_y_;

    statistics()
    {
        grid_width_ = 20;
        grid_height_ = 28;

        pixel_width_ = 180;
        pixel_height_ = 180;

        url_nature_ = "overworld_nature.png";
        url_border_ = "overworld_faction_border.png";
        url_factions_ = "overworld_faction_structures.png";
        url_structures_ = "overworld_neutral_structures.png";

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

        screen_width_ = 1280;
        screen_height_= 720;

        finished_nature.push_back(4);//grass
        finished_nature.push_back(4);//hill
        finished_nature.push_back(4);//lake
        finished_nature.push_back(4);//rock

        finished_border.push_back(6);//border_red
        finished_border.push_back(6);//border_green
        finished_border.push_back(6);//border_blue

        finished_factions.push_back(3);//red city
        finished_factions.push_back(3);//green city
        finished_factions.push_back(3);//blue city

        finished_structures.push_back(3);//mountains
        finished_structures.push_back(1);//forrest
        finished_structures.push_back(1);//unused
        finished_structures.push_back(1);//tavern
        finished_structures.push_back(1);//dungeon
        finished_structures.push_back(1);//ruin
        finished_structures.push_back(1);//caravan
    }
};
} // namespace overworld
} // namespace icarus
#endif // STATISTICS_H
