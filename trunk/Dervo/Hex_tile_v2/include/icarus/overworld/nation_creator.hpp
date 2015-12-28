#ifndef NATION_CREATOR_H
#define NATION_CREATOR_H

#include "statistics.hpp"
#include "icarus/math/vector2.hpp"
#include "battlezone.hpp"
#include "hex_sprite.hpp"
#include "nation_info.hpp"
#include <vector>
namespace icarus
{
namespace overworld
{

class nation_creator
{
private:
    statistics stats;
    std::vector<double*> reputation_pointers;
protected:
    //derp
public:
    nation_creator();
    virtual ~nation_creator();
    void setup_nations(); // runn before nation_reference is set.
    void handle_nations(std::vector<nation_info>& nations_information_); // runn every time player enters a new tile.
    void get_nation_list();
    void get_nations_reference(std::vector<nation_info>& nations, std::vector<hex_sprite*>& fraction, overworld::type_frac type);
    double get_nation_reputation_value(overworld::type_frac id);
};

} // namespace overworld
} // namespace icarus
#endif // NATION_CREATOR_H
