#include "icarus/overworld/encounter_calculator.hpp"
namespace icarus
{
namespace overworld
{
encounter_calculator::encounter_calculator(utilities::yth_node* data)
{
    //ctor
    encounter_list = data;

}

encounter_calculator::~encounter_calculator()
{
    //dtor
}
void encounter_calculator::setup_encounter(hex_sprite* encounter_on)//data is whole encounter list
{
    std::vector<struct_list> possible_encounters;
    struct_list push_into;
    int current_value = 0;
    overworld::nation tile_nation;
    overworld::nature tile_nature;
    for(unsigned per_data=0;per_data < data->child_count(); per_data++)
    {
        std::string string_case = data->child(2)->child(0)->value().c_str();
        if(string_case == "ALL")
        {
            push_into.encounter = data->child(per_data);
            current_value +=  atoi(data->child(per_data)->child(3)->value().c_str());
//            current_value +=  atoi(data(per_data)->child(3).c_str());
            push_into.value = current_value;
            possible_encounters.push_back(push_into);
        }
        else if(string_case == "SOCIAL")
        {
            push_into.encounter = data->child(per_data);
            current_value +=  atoi(data->child(per_data)->child(3)->value().c_str());
            push_into.value = current_value;
            possible_encounters.push_back(push_into);
        }
        else if(string_case == "FORREST"){ tile_nature = overworld::nature::FORREST_; }
        else if(string_case == "HILL")
        {
            tile_nature = overworld::nature::HILL_;
        }
        else if(string_case == "LAKE")
        {
            tile_nature = overworld::nature::LAKE_;
        }
        else if(string_case == "NATION1")
        {
            tile_nation = overworld::nation::RED_;
        }
        else if(string_case == "NATION2")
        {
            tile_nation = overworld::nation::BLUE_;
        }
        else if(string_case == "NATION3")
        {
            tile_nation = overworld::nation::GREEN_;
        }
        else if(string_case == "NEUTRAL")
        {
            tile_nation = overworld::nation::NEUTRAL_;
        }

        if(encounter_on->get_nation() == tile_nation)
        {
            push_into.encounter = data->child(per_data);
            current_value +=  atoi(data->child(per_data)->child(3)->value().c_str());
            push_into.value = current_value;
            possible_encounters.push_back(push_into);
        }
        if(encounter_on->get_nature() == tile_nature)
        {
            push_into.encounter = data->child(per_data);
            current_value +=  atoi(data->child(per_data)->child(3)->value().c_str());
            push_into.value = current_value;
            possible_encounters.push_back(push_into);
        }
    }

    //which encounter
    unsigned number = rand() % current_value;
    for(unsigned per_value=0;per_value < possible_encounters.size();per_value++)
    {
        if(possible_encounters[per_value].value >= number)
        {
            //this is the encounter
            std::cout << "encounter calculared" << std::endl;
        }
    }
}

} // namespace overworld
} // namespace icarus
