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

void nation_creator::get_nations_reference(std::vector<nation_info>& nations, std::vector<hex_sprite*>& fraction, overworld::nation type)
{
    /*
    nation_info temp = nation_info(type, fraction);
    temp.generate_campaign();
    //std::cout << "  push_back and end of function" << std::endl;
    SATAN.push_back(temp);
    //nations_information_.push_back(temp);
    */
    nations.push_back(nation_info(type, fraction));
    nations[nations.size()-1].generate_campaign();
}
} // namespace overworld
} // namespace icarus
