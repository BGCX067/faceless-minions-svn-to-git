#include "icarus/overworld/nation_info.hpp"
namespace icarus
{
namespace overworld
{
nation_info::nation_info(overworld::type_frac type_nation, typedef_tile& tiles, std::vector<sf::Vector2i>& sprites) :
            type_nation_(type_nation),
            nation_tiles_(tiles),
            buildcoins(0),
            sprites_(sprites),
            stats(statistics::get()),
            is_alive(true),
            has_building_projects(false)
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
        for(unsigned tile = 0; tile < (*nation_tiles_[number]).get_surrounding_tile_length(); tile++)
            if((*nation_tiles_[number]).get_surrounding_tile(tile)->get_nation() != type_nation_)
                possible_targets.push_back((*nation_tiles_[number]).get_surrounding_tile(tile));

    unsigned number;
    do
    {
        number = rand() % possible_targets.size();
    }
    while(possible_targets[number]->return_siege() != false);

    //why is this one here?
    //add_tile(possible_targets[number]);
    std::cout << "CAMPAIGN GENERATES NAOW" << std::endl;
    siege_zone_.set_battle(possible_targets[number], type_nation_, possible_targets[number]->get_nation());
}
void nation_info::continue_campaign(hex_sprite* battle_at, unsigned delay)
{
    std::cout << "RETURN TO NATION " << std::endl;
    std::cout << "      DEFENDER " << battle_at->get_nation() << std::endl;
    std::cout << "      ATTACKER " << type_nation_<< std::endl;
    siege_zone_.set_battle(battle_at, type_nation_, battle_at->get_nation(), delay);
    std::cout << "return_to_battle derp" << std::endl;
}
void nation_info::return_to_battle(hex_sprite* battlepoint, unsigned attacking_nation, unsigned prebattle)
{
    siege_zone_.set_battle(battlepoint, static_cast<type_frac>(attacking_nation), battlepoint->get_nation(), prebattle);
}
///add_tile(hex_sprite* tile) is used when gained a new tile to the nation
void nation_info::add_tile(hex_sprite* tile)
{
    tile->set_nation(type_nation_);
    tile->update_border(true);
    nation_tiles_.push_back(tile);
}

void nation_info::next_step()
{
    if(is_alive)
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
    else
    {
        if(siege_zone_.is_active())
            siege_zone_.remove_siege();
    }
}
void nation_info::run_cleanup(unsigned num_rounds)
{
    if(is_alive)
    {
        if(num_rounds % 5)
            handle_building();
        else
            handle_building(true);


        unsigned i =0;
        while(i < nation_tiles_.size())
        {
            if((*nation_tiles_[i]).get_nation() == type_nation_)
                i++;
            else
                nation_tiles_.erase(nation_tiles_.begin()+i);
        }
    }
}
void nation_info::setup_reputation_pointers(std::vector <double*> other_reputation)
{
    for(unsigned i=0;i< other_reputation.size();i++)
        if(i != static_cast<unsigned>(type_nation_))
            enemy_reputation.push_back(other_reputation[i]);
}
void nation_info::change_reputation(bool is_friendly, double amount)
{
    if(is_friendly)
        reputation_score += amount;
    else
        for(unsigned i=0;i<enemy_reputation.size();i++)
            *enemy_reputation[i] += amount;
}
void nation_info::add_building(unsigned col, unsigned row,
                               std::vector<typedef_row>& grid,
                               unsigned type_structure,
                               unsigned rounds_left)
{
    building::building_progress temp;
    temp.finished_structure = static_cast<type_overlay>(type_structure);
    temp.tile_ = &grid[col][row];
    temp.num_rounds_left = rounds_left;
    has_building_projects = true;
    building_projects.push_back(temp);
}
void nation_info::handle_building(bool build)
{
    std::vector<building::importance_list> temp_list;
    float increasing =0;
    for(unsigned i=0;i<nation_tiles_.size();i++)
    {
        buildcoins += stats->buildcoin_tile;
        switch(nation_tiles_[i]->get_overlay())
        {
            case overworld::type_overlay::CAPITAL_:     buildcoins += (stats->buildcoin_structure *3);break;
            case overworld::type_overlay::CITY_:        buildcoins += (stats->buildcoin_structure *2);break;
            case overworld::type_overlay::SETTLEMENT_:  buildcoins += (stats->buildcoin_structure   );break;
            case overworld::type_overlay::TAVERN_ :     buildcoins += (stats->buildcoin_structure   );break;
            //case overworld::type_overlay::CASTLE_:      buildcoins -= (stats.buildcoin_expences  *3);break;
            //case overworld::type_overlay::FORTRESS_:    buildcoins -= (stats.buildcoin_expences  *2);break;
            //case overworld::type_overlay::TOWER_ :      buildcoins -= (stats.buildcoin_expences  *2);break;
            //case overworld::type_overlay::OUTPOST_:     buildcoins -= (stats.buildcoin_expences    );break;
            default:break;
        }
        if(build && (nation_tiles_[i]->get_overlay() == SETTLEMENT_ ||
                     nation_tiles_[i]->get_overlay() == FORTRESS_ ||
                     nation_tiles_[i]->get_overlay() == TOWER_ ||
                     nation_tiles_[i]->get_overlay() == OUTPOST_ ||
                     nation_tiles_[i]->get_overlay() == TYPE_OVERLAY_NONE  ||
                     nation_tiles_[i]->get_overlay() == FORREST_) )
        {
            building::importance_list temp;
            temp.value_ = (increasing += nation_tiles_[i]->get_importance());
            temp.tile_ = nation_tiles_[i];
            temp_list.push_back(temp);
        }
    }
    if(build && temp_list.size() >0)
    {
        float number = (static_cast<float>(rand() % (static_cast<unsigned>(increasing)? static_cast<unsigned>(increasing):1)));
        for(unsigned i=temp_list.size()-1; i > 0; i--)
            if(temp_list[i].value_ < number)
            {
                unsigned needed_coinage = 0;
                building::buildings builded;
                switch(temp_list[i].tile_->get_overlay())
                {
                case SETTLEMENT_:       needed_coinage =  8;builded = building::buildings::b_city; break;// make city
                case FORTRESS_:         needed_coinage = 10;builded = building::buildings::b_castle;break;// make castle
                case TOWER_ :           needed_coinage = 10;builded = building::buildings::b_castle;break;// make castle
                case OUTPOST_:          needed_coinage =  8;builded = (rand()%2) ? building::buildings::b_tower : building::buildings::b_fortress;break;// make either tower or fortress
                case FORREST_:          needed_coinage =  6;builded = building::buildings::b_outpost;break;//make outpost
                case TYPE_OVERLAY_NONE: needed_coinage =  4;builded = building::buildings::b_outpost;break;// make outpost
                default:break;
                }

                if(static_cast<float>(needed_coinage) < buildcoins)
                {
                    buildcoins -= static_cast<float>(needed_coinage);
                    building::building_progress temp;
                    unsigned derp = rand() % 3;
                    overworld::type_overlay overlay;
                    switch(builded)
                    {
                    case building::buildings::b_city:    derp += build_city_1;      overlay = CITY_;    break;
                    case building::buildings::b_castle:  derp += build_castle_1;    overlay = CASTLE_;  break;
                    case building::buildings::b_tower:   derp += build_tower_1;     overlay = TOWER_;   break;
                    case building::buildings::b_fortress:derp += build_fort_1;      overlay = FORTRESS_;break;
                    case building::buildings::b_outpost: derp += build_post_1;      overlay = OUTPOST_; break;
                    }
                    temp.finished_structure = overlay;
                    temp.tile_ = temp_list[i].tile_;
                    temp.num_rounds_left = needed_coinage*2;
                    has_building_projects = true;
                    building_projects.push_back(temp);

                    temp_list[i].tile_->set_overlay(overworld::type_overlay::UNDER_CONSTRUCTION);
                    sf::Vector2i cord = sprites_[derp];
                    temp_list[i].tile_->set_sprite_coordinates(&cord, overworld::type_texture::OVERLAY_);
                }
                break;
            }
    }
    std::vector<unsigned> remove_list;
    for(unsigned i=0;i<building_projects.size();i++)
    {
        if(building_projects[i].num_rounds_left == 0)
        {
            unsigned item = rand() % 3;
            switch(building_projects[i].finished_structure)
            {
                case CASTLE_:   item += CASTLE_1;   break;
                case FORTRESS_: item += FORTRESS_1; break;
                case TOWER_ :   item += TOWER_1;    break;
                case OUTPOST_:  item += OUTPOST_1;  break;
                case CITY_:     item += CITY_1;     break;
                default: break;
            }
            sf::Vector2i cord = sprites_[item];
            building_projects[i].tile_->set_sprite_coordinates(&cord, overworld::type_texture::OVERLAY_);
            building_projects[i].tile_->set_overlay(building_projects[i].finished_structure);
            building_projects[i].tile_->set_is_used(true);
            remove_list.push_back(i);
        }
        else
            building_projects[i].num_rounds_left --;
    }
    for(unsigned i=remove_list.size();i>0;i--)
        building_projects.erase(building_projects.begin()+remove_list[i-1]);
}
unsigned nation_info::get_delay(hex_sprite* tile)
{
    if(tile->return_siege() )
    {
        return siege_zone_.get_round_count();
    }
    if(has_building_projects)
        for(unsigned i=0;i<building_projects.size();i++)
        {
            if(tile == building_projects[i].tile_)
                return building_projects[i].num_rounds_left;
        }
    return 0;
}
} // namespace overworld
} // namespace icarus
