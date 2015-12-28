#include <iostream>
#include <ctime>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "icarus/state_manager.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/encounter/state.hpp"

int main()
{
    srand(0xabcdef);
    icarus::state_manager::get()->initialize("settings.cfg");

    // START DEBUG DATA -----
    icarus::encounter::data test_encounter;

    icarus::encounter::hero_data p1data;
    p1data.name_ = "TestChar 1";
    p1data.initial_position_ = icarus::math::vector2i(0, 0);
    p1data.skill_levels_[0] = 4;
    p1data.skill_levels_[1] = 4;
    p1data.skill_levels_[2] = 4;
    p1data.skill_levels_[3] = 1;
    p1data.health_ = 100;
    p1data.current_health_ = 1.0f;
    p1data.strength_ = 20;
    p1data.agility_ = 20;
    p1data.intelligence_ = 20;
    p1data.move_speed_ = 300.0f;
    p1data.class_ = icarus::encounter::hero_class::mage;
    test_encounter.player_party_.push_back(p1data);

    icarus::encounter::hero_data p2data;
    p2data.name_ = "TestChar 2";
    p2data.initial_position_ = icarus::math::vector2i(7, 0);
    p2data.skill_levels_[0] = 4;
    p2data.skill_levels_[1] = 4;
    p2data.skill_levels_[2] = 4;
    p2data.skill_levels_[3] = 1;
    p2data.health_ = 100;
    p2data.current_health_ = 1.0f;
    p2data.strength_ = 100;
    p2data.agility_ = 100;
    p2data.intelligence_ = 100;
    p2data.move_speed_ = 300.0f;
    p2data.class_ = icarus::encounter::hero_class::mage;
    test_encounter.player_party_.push_back(p2data);

    test_encounter.encounter_node_ = icarus::resource_handler::get()->get_root_node("encounters")->child("region", icarus::resource_handler::get()->get_root_node("encounters")->child_count("region") - 1)->child("test_encounter")->child("combat");
    test_encounter.current_scenario_ = 0;
    test_encounter.scenario_count_ = test_encounter.encounter_node_->child_count("scenario");

    icarus::state_manager::get()->store_data(&test_encounter);
    // END DEBUG DATA -----

    icarus::state_manager::get()->start();

    return 0;
}
