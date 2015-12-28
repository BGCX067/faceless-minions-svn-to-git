#ifndef NATION_INFO_H
#define NATION_INFO_H

#include "hex_sprite.hpp"
#include "statistics.hpp"
#include "icarus/math/vector2.hpp"
#include "battlezone.hpp"
#include <vector>


namespace icarus
{
namespace building
{
struct building_progress
{
    overworld::hex_sprite* tile_;
    overworld::type_overlay finished_structure;
    unsigned num_rounds_left;
};
struct importance_list
{
    overworld::hex_sprite* tile_;
    float value_;
};
enum buildings
{
    b_city,
    b_castle,
    b_tower,
    b_fortress,
    b_outpost
};

}

namespace overworld
{
class nation_info
{
private:
    typedef std::vector<hex_sprite*>         typedef_tile;
    typedef std::vector<hex_sprite>          typedef_row;
    overworld::type_frac                     type_nation_;
    typedef_tile                             nation_tiles_;
    float                                    buildcoins;
    std::vector<sf::Vector2i>                sprites_;

    std::vector <double*>                    enemy_reputation;
    std::vector<building::building_progress> building_projects;
    statistics*                              stats;
    hex_sprite*                              attacking_tile;
    battlezone                               siege_zone_;
    double                                   reputation_score;
    bool                                     is_alive;
    bool                                     has_building_projects;

protected:
    //derp
public:
    nation_info(overworld::type_frac type_nation, typedef_tile& tiles, std::vector<sf::Vector2i>& sprites);
    //nation_info();
    virtual ~nation_info();
    void generate_campaign();//run when battlezone is activated.
    void return_to_battle(hex_sprite* battlepoint, unsigned attacking_nation, unsigned prebattle);
    void next_step();//run every turn
    void set_attacker(hex_sprite* tile = NULL);
    //void set_tiles(std::vector<hex_sprite*>& tiles);
    void add_tile(hex_sprite* tile);
    void run_cleanup(unsigned num_rounds);
    void setup_reputation_pointers(std::vector <double*> other_reputation);
    void change_reputation(bool is_friendly, double amount);
    void handle_building(bool build = false);
    void add_building(unsigned col, unsigned row, std::vector<typedef_row>& grid, unsigned type_structure, unsigned rounds_left);
    unsigned get_delay(hex_sprite* tile);
    void continue_campaign(hex_sprite* battle_at, unsigned delay);

    double* setup_reputation()      {reputation_score = 50.00; return &reputation_score;}
    void    refresh_army_value()    {statistics::get()->army_size_per_nation[type_nation_] = (statistics::get()->army_size / nation_tiles_.size()? nation_tiles_.size(): 1);}//  nation_tiles_.size());}
    double  get_reputation()        {return reputation_score;}
    float get_build_coins() const   {return buildcoins;}
    unsigned get_attacker()         {return siege_zone_.get_attacker();}
    void clear_buildin_queue()      {while(building_projects.size()>0){building_projects.erase(building_projects.begin());};}
    bool has_lost()                 {return !is_alive;}
    void set_loss()                 {is_alive = false;}
    void deactivate_battle()        {siege_zone_.remove_siege();}
    void add_to_nation_list(hex_sprite* i) {nation_tiles_.push_back(i);}

};

} // namespace overworld
} // namespace icarus

#endif // NATION_INFO_H
