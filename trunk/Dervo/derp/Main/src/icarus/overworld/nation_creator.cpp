#include "icarus/overworld/nation_creator.hpp"
namespace icarus
{
namespace overworld
{

nation_creator::nation_creator()
{
    //ctor
}

nation_creator::~nation_creator()
{
    //dtor
}
void nation_creator::setup_nations()
{
    //temporarly 3 nations is hardcoded into the game
}
void nation_creator::handle_nations(std::vector<nation_info>& nations_information_)
{
    for(unsigned i =0;i<nations_information_.size();i++)
    {
        nations_information_[i].next_step();
        nations_information_[i].run_cleanup();
    }
}

void nation_creator::get_nations_reference(std::vector<nation_info>& nations, std::vector<hex_sprite*>& fraction, overworld::type_frac type)
{
    nations.push_back(nation_info(type, fraction));
    nations[nations.size()-1].generate_campaign();
    reputation_pointers.push_back(nations[nations.size()-1].setup_reputation());

    if(reputation_pointers.size() == 3)
    {
        for(unsigned i=0;i<reputation_pointers.size();i++)
        {
            nations[i].setup_reputation_pointers(reputation_pointers);
        }
    }
}
double nation_creator::get_nation_reputation_value(overworld::type_frac id)
{
    return *reputation_pointers[id];
}
} // namespace overworld
} // namespace icarus
