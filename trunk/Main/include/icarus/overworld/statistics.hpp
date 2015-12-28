#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <string>
#include "icarus/utils.hpp"

namespace icarus
{
namespace overworld
{
class hex_sprite;
enum type_          {T_FRACTION_, T_GROUND_TILE_, T_OVERLAY_};
enum type_frac      {RED_, GREEN_, BLUE_, SPAWN_, TYPE_FRAC_NONE};

enum type_ground    {PLANES_, ROCK_, FORREST_FLOOR_, WATTER_, TYPE_GROUND_EMPTY, TYPE_GROUND_NO_USE};

enum type_overlay   {   MOUNTAIN_,          /// 0
                        FORREST_,           /// 1
                        HILL_,              /// 2
                        SETTLEMENT_,        /// 3
                        TAVERN_,            /// 4
                        CITY_,              /// 5
                        RUIN_,              /// 6
                        DUNGEON_,           /// 7
                        CASTLE_,            /// 8
                        FORTRESS_,          /// 9
                        TOWER_,             /// 10
                        OUTPOST_,           /// 11
                        CAPITAL_,           /// 12
                        UNDER_CONSTRUCTION, /// 13
                        TYPE_OVERLAY_NONE,  /// 14
                        TYPE_OVERLAY_NO_USE};///15

enum type_texture   {BASE_IMAGE,
		F_BORDER_0, F_BORDER_1, F_BORDER_2, F_BORDER_3, F_BORDER_4, F_BORDER_5,
		N_BORDER_0, N_BORDER_1, N_BORDER_2, N_BORDER_3, N_BORDER_4, N_BORDER_5,
		OVERLAY_};
enum state          {UNREACHABLE_, PLAYER_, VISITABLE_, OCUPIED_, VISIBLE_};


enum sheet_ground{  GRASS_1,GRASS_2,GRASS_3,GRASS_4,GRASS_5,
                    GRASS_6,GRASS_7,GRASS_8,GRASS_9,

                    ROCK_1, ROCK_2, ROCK_3, ROCK_4, ROCK_5,
                    ROCK_6, ROCK_7, ROCK_8, ROCK_9,

                    WODS_1, WODS_2, WODS_3, WODS_4, WODS_5,
                    WODS_6, WODS_7, WODS_8, WODS_9,

                    WATER_1, WATER_2, WATER_3, WATER_4, WATER_5,
                    WATER_6, WATER_7, WATER_8, WATER_9,

                    sheet_ground_none};

enum sheet_border{  B_RED_1,  B_RED_2,  B_RED_3,  B_RED_4,  B_RED_5,  B_RED_6,
                    B_GREEN_1,B_GREEN_2,B_GREEN_3,B_GREEN_4,B_GREEN_5,B_GREEN_6,
                    B_BLUE_1, B_BLUE_2, B_BLUE_3, B_BLUE_4, B_BLUE_5, B_BLUE_6,
                    N_ROCK_1, N_ROCK_2, N_ROCK_3, N_ROCK_4, N_ROCK_5, N_ROCK_6,
                    N_FORE_1, N_FORE_2, N_FORE_3, N_FORE_4, N_FORE_5, N_FORE_6,
                    N_WATE_1, N_WATE_2, N_WATE_3, N_WATE_4, N_WATE_5, N_WATE_6,
                    sheet_border_none};

enum sheet_overlays{    MOUNTAIN_1,     MOUNTAIN_2,     MOUNTAIN_3,             // MOUNTAIN_
                        FORREST_1,      FORREST_2,      FORREST_3,              // FORREST_
                        HILLS_1,        HILLS_2,        HILLS_3,                // HILL_

                        SETTLEMENT_1,   SETTLEMENT_2,   SETTLEMENT_3,           // SETTLEMENT_
                        TAVERN_1,       TAVERN_2,       TAVERN_3,               // TAVERN_
                        CITY_1,         CITY_2,         CITY_3,                 // CITY_

                        RUIN_1,         RUIN_2,         RUIN_3,                 // RUIN_
                        DUNGEON_1,      DUNGEON_2,      DUNGEON_3,              // DUNGEON_
                        CASTLE_1,       CASTLE_2,       CASTLE_3,               // CASTLE_

                        FORTRESS_1,     FORTRESS_2,     FORTRESS_3,             // FORTRESS_
                        TOWER_1,        TOWER_2,        TOWER_3,                // TOWER_
                        OUTPOST_1,      OUTPOST_2,      OUTPOST_3,              // OUTPOST_

                        CAP_RED_1,      CAP_RED_2,      CAP_RED_3,
                        CAP_GREEN_1,    CAP_GREEN_2,    CAP_GREEN_3,
                        CAP_BLUE_1,     CAP_BLUE_2,     CAP_BLUE_3,

                        build_city_1,   build_city_2,   build_city_3,
                        build_castle_1, build_castle_2, build_castle_3,
                        build_tower_1,  build_tower_2,  build_tower_3,

                        build_fort_1,   build_fort_2,   build_fort_3,
                        build_post_1,   build_post_2,   build_post_3,
                        SPAWN_POINT,    so_empty,       };

enum reputation_methre{HOSTILE_, INDIFFERENT_, FRIENDLY_, NONFRAC_};
//enum siege {PREBATTLE_, BATTLE_};



class statistics
{
private:
    static statistics* instance_;

    statistics()
    {
        grid_width_ = 20;
        grid_height_ = 28;

        pixel_width_ = 180;
        pixel_height_ = 180;
        player_start_x = 0;
        player_start_y = 0;

        url_base_ = "images/overworld/overworld_nature.png";
        url_borders_ = "images/overworld/overworld_faction_border.png";
        url_overlay_ = "images/overworld/overworld_neutral_structures.png";

        hex_offset_ = 99;

        num_fractions_ = 3+1;

        url_character_sprite_ = "images/overworld/sprite_char2.png";

        character_pixel_height_ = 75;
        character_pixel_width_ = 56;

        hex_sprite_margin = 81;

        army_size = 0.5f;
        battle_modifier = 0.5f;

        army_size_per_nation.push_back(0.0f);
        army_size_per_nation.push_back(0.0f);
        army_size_per_nation.push_back(0.0f);

        screen_width_ = utils::to_hd(1280);
        screen_height_= utils::to_hd(720);

        finished_ground.push_back(4);//PLANES_
        finished_ground.push_back(4);//ROCK_
        finished_ground.push_back(4);//FORREST_FLOOR_
        finished_ground.push_back(4);//WATTER_

        finished_overlay.push_back(3);//mountains
        finished_overlay.push_back(3);//forrest
        finished_overlay.push_back(3);//high_hills
        finished_overlay.push_back(3);//settlement
        finished_overlay.push_back(3);//tavern
        finished_overlay.push_back(3);//cities
        finished_overlay.push_back(3);//ruin
        finished_overlay.push_back(3);//dungeon
        finished_overlay.push_back(3);//castles
        finished_overlay.push_back(3);//fotresses
        finished_overlay.push_back(3);//towers
        finished_overlay.push_back(3);//outpost

        chance_differance = 300;

        reputation_numbers.push_back(1.00);//helped_battle,
        reputation_numbers.push_back(-2.0);//hostile_battle,
        reputation_numbers.push_back(-1.0);//helped_hostile,
        reputation_numbers.push_back(0.5);//attack_hostile_city,
        reputation_numbers.push_back(0.8);//attacked_hostile_capital,
        reputation_numbers.push_back(1.0);//gained_fraction_tile,
        reputation_numbers.push_back(1.5);//gained_fraction_city,
        reputation_numbers.push_back(1.5);//gained_fraction_capital,
        reputation_numbers.push_back(-2.0);//attacked_fraction_city,
        reputation_numbers.push_back(-3.0);//attacked_fraction_captial};

        reputation_values.push_back(40);//below equals hostile
        reputation_values.push_back(60);//between 60-40 neutral
        reputation_values.push_back(100);//betewwn 60-100 friendly

        //uses overlay values, thus not al enteties have values.
        local_presense.push_back(0);    regional_presense.push_back(0);     // MOUNTAIN_
        local_presense.push_back(0);    regional_presense.push_back(0);     // FORREST_
        local_presense.push_back(0);    regional_presense.push_back(0);     // HILL_
        local_presense.push_back(0.1);  regional_presense.push_back(0);     // SETTLEMENT_
        local_presense.push_back(0.5);  regional_presense.push_back(0.1);   // TAVERN_
        local_presense.push_back(1);    regional_presense.push_back(0.1);   // CITY_
        local_presense.push_back(0);    regional_presense.push_back(0);     // RUIN_
        local_presense.push_back(0);    regional_presense.push_back(0);     // DUNGEON_
        local_presense.push_back(2);    regional_presense.push_back(0.25);  // CASTLE_
        local_presense.push_back(1.5);  regional_presense.push_back(0.15);  // FORTRESS_
        local_presense.push_back(1);    regional_presense.push_back(0.1);   // TOWER_
        local_presense.push_back(0.5);  regional_presense.push_back(0);     // OUTPOST_
        local_presense.push_back(2.5);  regional_presense.push_back(0.3);   // CAPITAL_

        buildcoin_tile = 0.005;
        buildcoin_structure = 0.005;
        buildcoin_battle = 0.05;
        buildcoin_expences = 0.005;
    }
    statistics(statistics const&);
    void operator=(statistics const&);

public:
    std::vector<unsigned> finished_ground;
    std::vector<unsigned> finished_overlay;
    // Hex_sprite
    unsigned grid_width_;
    unsigned grid_height_;
    unsigned pixel_width_;
    unsigned pixel_height_;
    unsigned hex_offset_;
    unsigned num_fractions_;

    std::string url_base_;
    std::string url_borders_;
    std::string url_overlay_;

    unsigned  hex_sprite_margin;

    //character
    std::string url_character_sprite_;
    unsigned character_pixel_width_;
    unsigned character_pixel_height_;
    unsigned player_start_x;
    unsigned player_start_y;

    //battle
    float army_size;
    float battle_modifier;
    std::vector <float> army_size_per_nation;

    std::vector<float> local_presense;
    std::vector<float> regional_presense;

    //screen
    float screen_width_;
    float screen_height_;

    float scenter_x_;
    float scenter_y_;

    unsigned chance_differance;

    std::vector<double> reputation_numbers;

    std::vector<int> reputation_values;


    float buildcoin_tile;
    float buildcoin_structure;
    float buildcoin_battle;
    float buildcoin_expences;

    static statistics* get()
    {
        if (!instance_)
            instance_ = new statistics();
        return instance_;
    }
};
} // namespace overworld

namespace battle_info
{
    struct battlezone_info
    {
        unsigned                col;
        unsigned                row;
        overworld::type_frac    attacker;
        unsigned                battle_delay;
    };
}
} // namespace icarus
#endif // STATISTICS_H