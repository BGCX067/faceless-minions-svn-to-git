#include "icarus/overworld/nation_creator.hpp"
namespace icarus
{
namespace overworld
{

nation_creator::nation_creator() : stats(statistics::get()),
                                    num_rounds(0)
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
    for(unsigned i =1;i<nations_information_.size();i++)
    {
        nations_information_[i].next_step();
        nations_information_[i].run_cleanup(num_rounds);
    }
    num_rounds++;
}
void nation_creator::setup_nations(std::vector<nation_info>& nations,
                                           std::vector<hex_sprite*>& fraction,
                                           overworld::type_frac type,
                                           std::vector<sf::Vector2i>& sprites)
{
    nations.push_back(nation_info(type, fraction, sprites));
    nations[nations.size()-1].generate_campaign();
    reputation_pointers.push_back(nations[nations.size()-1].setup_reputation());

    if(reputation_pointers.size() == 3)
        for(unsigned i=0;i<reputation_pointers.size();i++)
            nations[i].setup_reputation_pointers(reputation_pointers);
}
void nation_creator::setup_nations_from_save(std::vector<nation_info>& nations,
                                             std::vector<hex_sprite*>& fraction,
                                           overworld::type_frac type,
                                           std::vector<sf::Vector2i>& sprites)
{
    nations.push_back(nation_info(type, fraction, sprites));

    reputation_pointers.push_back(nations[nations.size()-1].setup_reputation());

    if(reputation_pointers.size() == 3)
        for(unsigned i=0;i<reputation_pointers.size();i++)
            nations[i].setup_reputation_pointers(reputation_pointers);
}
void nation_creator::set_battlezones_from_save(std::vector<hex_hold_>& grid,
                                               std::vector<nation_info>& nations,
                                               std::vector<battle_info::battlezone_info>& battlezones)
{
    for(unsigned i=0;i<battlezones.size();i++)
    {
        nations[battlezones[i].attacker].continue_campaign(&grid[battlezones[i].row][battlezones[i].col], battlezones[i].battle_delay);
    }
}
bool nation_creator::is_game_over(std::vector<nation_info>& nations)
{
    if(nations[0].has_lost() &&
       nations[1].has_lost() &&
       nations[2].has_lost() )
        return true;
    return false;
}
} // namespace overworld
} // namespace icarus
