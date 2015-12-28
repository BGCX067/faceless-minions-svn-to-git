#ifndef NATION_INFO_H
#define NATION_INFO_H

#include "hex_sprite.hpp"
#include "statistics.hpp"
#include "icarus/math/vector2.hpp"
#include "battlezone.hpp"
#include <vector>


namespace icarus
{
namespace overworld
{
class nation_info
{
private:
    statistics stats;
    hex_sprite* attacking_tile;
    overworld::type_frac type_nation_;
    typedef std::vector <hex_sprite*>   typedef_tile;
    typedef_tile nation_tiles_;
    battlezone siege_zone_;
    double reputation_score;
    std::vector <double*> enemy_reputation;
protected:
    //derp
public:
    nation_info(overworld::type_frac type_nation, typedef_tile& tiles);
    virtual ~nation_info();
    void generate_campaign();//run when battlezone is activated.
    void next_step();//run every turn
    void set_attacker(hex_sprite* tile = NULL);
    //void set_tiles(std::vector<hex_sprite*>& tiles);
    void add_tile(hex_sprite* tile);
    void run_cleanup();
    void refresh_army_value();
    double* setup_reputation();
    double get_reputation();
    void setup_reputation_pointers(std::vector <double*> other_reputation);
    void change_reputation(bool is_friendly, double amount);
};

} // namespace overworld
} // namespace icarus

#endif // NATION_INFO_H
