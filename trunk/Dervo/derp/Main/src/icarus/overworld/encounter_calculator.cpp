#include "icarus/overworld/encounter_calculator.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
namespace icarus
{
namespace overworld
{
encounter_calculator::encounter_calculator()
{

}

encounter_calculator::~encounter_calculator()
{
    //dtor
}
void encounter_calculator::setup_encounter(hex_sprite* encounter_on, nation_creator& nations)//encounter_list is whole encounter list
{
    std::vector<struct_list> possible_encounters;
    struct_list push_into;
    unsigned current_value = 0;
    std::string type_encounter;

    //override functionality
    /* if some variables stored per tile gives the right signal, an override will be done.
    This will mean the encounter list will be emptied and only one new encounter will be added.
    */

    // 1. find reputation.
    if(encounter_on->get_nation() < 3) // 3 = neutral, nonowned tiles.
    {
        double temp = nations.get_nation_reputation_value(encounter_on->get_nation());
        if(temp < stats.reputation_values[overworld::reputation_methre::HOSTILE_])
        {
            type_encounter = "hostile_";
        }
        else if(temp < stats.reputation_values[overworld::reputation_methre::INDIFFERENT_])
        {
            type_encounter = "neutral_";
        }
        else
        {
            type_encounter = "friendly_";
        }
        std::cout << type_encounter << " nation" << std::endl;
    }
    else
    {
        type_encounter = "nofrac_";
        std::cout << type_encounter << " nonowned" << std::endl;
    }
    // part 2. type tile.
    if(encounter_on->return_siege())
    {
        std::cout<< "battlezone" << std::endl;
        type_encounter += "battlezone";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::CITY_)
    {
        std::cout<< "city" << std::endl;
        if(type_encounter == "nofrac_")
            type_encounter = "neutral_city";
        else
            type_encounter += "city";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::TAVERN_)
    {
        std::cout<< "tavern" << std::endl;
        type_encounter += "tavern";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::DUNGEON_)
    {
        std::cout<< "dungeon" << std::endl;
        type_encounter = "dungeon";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::RUIN_)
    {
        std::cout<< "ruins" << std::endl;
        type_encounter = "ruin";
    }/*
    else if(encounter_on->get_overlay() == overworld::type_overlay::CARAVAN_)
    {
        std::cout<< "caravan" << std::endl;
        type_encounter = "caravan";
    }*/
    else if(encounter_on->get_overlay() == overworld::type_overlay::CASTLE_)
    {
        std::cout<< "castle" << std::endl;
        type_encounter += "castle";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::TOWER_)
    {
        std::cout<< "tower" << std::endl;
        type_encounter += "tower";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::FORTRESS_)
    {
        std::cout<< "fortress" << std::endl;
        type_encounter += "fortress";
    }
    else if(encounter_on->get_overlay() == overworld::type_overlay::OUTPOST_)
    {
        std::cout<< "outpost" << std::endl;
        type_encounter += "outpost";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::PLANES_)
    {
        std::cout<< "grass" << std::endl;
        type_encounter += "grass";
    }
    else if (encounter_on->get_nature() == overworld::type_overlay::HILL_)
    {
        std::cout<< "high_hills" << std::endl;
        type_encounter += "high_hills";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::WATTER_)
    {
        std::cout<< "lake" << std::endl;
        type_encounter += "lake";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::ROCK_)
    {
        std::cout<< "mountain" << std::endl;
        type_encounter += "mountain";
    }
    else if (encounter_on->get_nature() == overworld::type_ground::FORREST_FLOOR_)
    {
        std::cout<< "forrest" << std::endl;
        type_encounter += "forrest";
    }
    if(!encounter_on->get_override())
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
            std::cout << "encounter title is: " << possible_encounters[per_value].encounter->child("display_name")->value() << std::endl;
            std::cout << "encounter text is: " << possible_encounters[per_value].encounter->child("display_text")->value() << std::endl;
            if(possible_encounters[per_value].encounter->child("display_name")->value() == "Nothing happens")
                break;
            gui::gui_handler::get()->display_encounter_dialog(possible_encounters[per_value].encounter);
            break;
        }
    }
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
        possible_encounters.push_back(push_into);
    }
}
void encounter_calculator::setup_data_link(utils::yth_node* data, utils::yth_node* type_reference)
{
    encounter_list = data;
    reference_to = type_reference;
}
} // namespace overworld
} // namespace icarus
