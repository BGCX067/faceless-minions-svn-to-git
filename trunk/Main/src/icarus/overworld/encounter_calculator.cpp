#include "icarus/overworld/encounter_calculator.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
#include "icarus/overworld/party_handler.hpp"
namespace icarus
{
namespace overworld
{
encounter_calculator::encounter_calculator() :
    stats(statistics::get()),
    num_tiles_moved(0)
{

}

encounter_calculator::~encounter_calculator()
{
    //dtor
}
void encounter_calculator::setup_encounter(hex_sprite* encounter_on, nation_creator& nations)//encounter_list is whole encounter list
{
    std::cout << "SETUP_ENCOUNTER"<< std::endl;
    std::vector<struct_list> possible_encounters;
    struct_list push_into;
    unsigned current_value = 0;
    std::string type_encounter;
    std::string first_encounters;
    num_tiles_moved++;
    if(num_tiles_moved < 4)
    {
        switch(num_tiles_moved)
        {
            case 1: first_encounters = "encounter_nr_1"; break;
            case 2: first_encounters = "encounter_nr_2"; break;
            case 3: first_encounters = "encounter_nr_3"; break;
            default: break;
        }
    }

    //override functionality
    /* if some variables stored per tile gives the right signal, an override will be done.
    This will mean the encounter list will be emptied and only one new encounter will be added.
    */

    // 1. find reputation.
    if(encounter_on->get_nation() < 3) // 3 = neutral, nonowned tiles.
    {
        double temp = nations.get_nation_reputation_value(encounter_on->get_nation());
        if(temp < stats->reputation_values[overworld::reputation_methre::HOSTILE_])
        {
            type_encounter = "hostile_";
        }
        else if(temp < stats->reputation_values[overworld::reputation_methre::INDIFFERENT_])
        {
            type_encounter = "neutral_";
        }
        else
        {
            type_encounter = "friendly_";
        }
    }
    else
    {
        type_encounter = "nofrac_";
    }
    // part 2. type tile.
    if(first_encounters == "encounter_nr_1" ||
       first_encounters == "encounter_nr_2" ||
       first_encounters == "encounter_nr_3")
    {
        type_encounter = first_encounters;
    }
    else if(encounter_on->return_siege())
    {
        type_encounter += "battlezone";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::CITY_ )
    {
        if(type_encounter == "nofrac_")
            type_encounter = "neutral_city";
        else
            type_encounter += "city";

        type_encounter = "test_siege";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::TAVERN_ )
    {
        if(type_encounter == "nofrac_")
            type_encounter = "neutral_tavern";
        else
            type_encounter += "tavern";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::DUNGEON_ )
    {
        type_encounter = "dungeon";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::RUIN_ )
    {
        type_encounter = "ruin";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::CAPITAL_)
    {
        std::cout<< "capital" << std::endl;
        type_encounter = "siege_capital";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::CASTLE_ )
    {
        type_encounter += "castle";

        type_encounter = "test_siege";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::TOWER_)
    {
        type_encounter += "tower";

        type_encounter = "test_siege";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::FORTRESS_)
    {
        type_encounter += "fortress";

        type_encounter = "test_siege";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::OUTPOST_)
    {
        type_encounter += "outpost";

        type_encounter = "test_siege";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::PLANES_)
    {
        type_encounter += "grass";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::WATTER_)
    {
        type_encounter += "lake";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::ROCK_)
    {
        type_encounter += "mountain";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::FORREST_FLOOR_)
    {
        type_encounter += "forrest";
    }

    std::cout << "Derp " << type_encounter << std::endl;

    if(!encounter_on->get_override() && num_tiles_moved >3)
        for(unsigned per_data=0;per_data < encounter_list->child_count(); per_data++)
            if(encounter_list->child("region", per_data)->value() == "ALL")
                add_encounter_to_list(per_data, push_into, current_value,possible_encounters);

    for(unsigned per_data=0;per_data<encounter_list->child_count();per_data++)
        if(encounter_list->child("region", per_data)->value() == type_encounter)
            add_encounter_to_list(per_data, push_into, current_value,possible_encounters);

    //which encounter
    int number = (current_value != 0 ? rand() % current_value : 0);
    for(unsigned per_value=0;per_value < possible_encounters.size();per_value++)
    {
        if(possible_encounters[per_value].value >= number)
        {
            //std::cout << "encounter title is: " << possible_encounters[per_value].encounter->child("display_name")->value() << std::endl;
            //std::cout << "encounter text is: " << possible_encounters[per_value].encounter->child("display_text")->value() << std::endl;
            if(possible_encounters[per_value].encounter->child("display_name")->value() == "Nothing happens")
                break;
            if(type_encounter == "neutral_tavern")
                std::cout << type_encounter << std::endl;

            party_handler::get()->get_state().state_ = unsigned(utils::encounter_state::enter_encounter);
            party_handler::get()->get_state().region_ = unsigned(possible_encounters[per_value].region_index_);
            party_handler::get()->get_state().encounter_ = unsigned(possible_encounters[per_value].encounter_index_);
            party_handler::get()->get_state().scenario_ = unsigned(0);
            party_handler::get()->get_state().party_xp_ = unsigned(0);
            state_manager::get()->save_game();
            gui::gui_handler::get()->display_encounter_dialog(possible_encounters[per_value].encounter);
            return;
        }
    }
    party_handler::get()->get_state().state_ = unsigned(utils::encounter_state::no_encounter);
    state_manager::get()->save_game();
}
void encounter_calculator::add_encounter_to_list(
                                unsigned per_data,
                                struct_list push_into,
                                unsigned& current_value,
                                std::vector<struct_list>& possible_encounters )
{
    for (unsigned i = 0; i < encounter_list->child("region", per_data)->child_count(); ++i)
    {
        push_into.encounter = encounter_list->child("region", per_data)->child(i);
        current_value +=  atoi(encounter_list->child("region", per_data)->child(i)->child("possibility")->value().c_str());
        //current_value +=  atoi(data(per_data)->child(3).c_str());
        push_into.value = current_value;
        push_into.region_index_ = per_data;
        push_into.encounter_index_ = i;
        possible_encounters.push_back(push_into);
    }
}
void encounter_calculator::setup_data_link(utils::yth_node* data, utils::yth_node* type_reference, unsigned num_tiles)
{
    if(num_tiles>0)
        num_tiles_moved =num_tiles;
    encounter_list = data;
    reference_to = type_reference;
    std::cout << "setup_data_link " << num_tiles_moved << std::endl;
}
} // namespace overworld
} // namespace icarus
