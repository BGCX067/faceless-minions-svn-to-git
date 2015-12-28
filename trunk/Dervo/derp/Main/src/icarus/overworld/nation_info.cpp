#include "icarus/overworld/nation_info.hpp"
namespace icarus
{
namespace overworld
{
nation_info::nation_info(overworld::type_frac type_nation, typedef_tile& tiles) :
            type_nation_(type_nation),
            nation_tiles_(tiles)
{
    //ctor
}

nation_info::~nation_info()
{
    //dtor
}
void nation_info::generate_campaign()
{
    std::vector<hex_sprite*> possible_targets;
    //1. find all surrounding tiles,
    for(unsigned number=0; number < nation_tiles_.size();number++)
    {
        for(unsigned tile = 0; tile < (*nation_tiles_[number]).get_surrounding_tile_length(); tile++)
        {
            if((*nation_tiles_[number]).get_surrounding_tile(tile)->
                    get_nation() != type_nation_)
            {
                possible_targets.push_back((*nation_tiles_[number]).get_surrounding_tile(tile));
            }
        }
    }
    unsigned number;
    do
    {
        number = rand() % possible_targets.size();
    }
    while(possible_targets[number]->return_siege() != false);

    //why is this one here?
    //add_tile(possible_targets[number]);

    siege_zone_.set_battle(possible_targets[number], type_nation_, possible_targets[number]->get_nation());

}
///add_tile(hex_sprite* tile) is used when gained a new tile to the nation
void nation_info::add_tile(hex_sprite* tile)
{
    //std::cout << type_nation_ << ", " << nation_tiles_.size() <<std::endl;
    tile->set_nation(type_nation_);
    nation_tiles_.push_back(tile);
    //std::cout<< nation_tiles_.size() <<std::endl;
}

void nation_info::next_step()
{
    refresh_army_value();
    if(siege_zone_.is_active())
    {
        if(siege_zone_.run_battle())
        {
            if(siege_zone_.return_winner())
            {
                add_tile(siege_zone_.return_siege_point());
                generate_campaign();
            }
            else
            {
                generate_campaign();
            }
        }
    }
}
void nation_info::run_cleanup()
{
    unsigned i =0;
    while(i < nation_tiles_.size())
    {
        if((*nation_tiles_[i]).get_nation() == type_nation_)
        {
            i++;
        }
        else
        {
            nation_tiles_.erase(nation_tiles_.begin()+i);
        }
    }
}
void nation_info::refresh_army_value()
{
    stats.army_size_per_nation[type_nation_] = (stats.army_size / nation_tiles_.size());
}

double* nation_info::setup_reputation()
{
    reputation_score = 50.00;
    return &reputation_score;
}
void nation_info::setup_reputation_pointers(std::vector <double*> other_reputation)
{
    for(unsigned i=0;i< other_reputation.size();i++)
        if(i != static_cast<unsigned>(type_nation_))
            enemy_reputation.push_back(other_reputation[i]);
}
double nation_info::get_reputation()
{
    return reputation_score;
}
void nation_info::change_reputation(bool is_friendly, double amount)
{
    if(is_friendly)
    {
        reputation_score += amount;
    }
    else
    {
        for(unsigned i=0;i<enemy_reputation.size();i++)
        {
            *enemy_reputation[i] += amount;
        }
    }
}
} // namespace overworld
} // namespace icarus
