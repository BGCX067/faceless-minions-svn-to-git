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
    statistics* stats;
    unsigned num_rounds;
    std::vector<double*> reputation_pointers;
    typedef std::vector<hex_sprite>     hex_hold_;

protected:
    //derp
public:
    nation_creator();
    virtual ~nation_creator();
    void setup_nations(); // runn before nation_reference is set.
    void handle_nations(std::vector<nation_info>& nations_information_); // runn every time player enters a new tile.
    void get_nation_list();
    void setup_nations(std::vector<nation_info>& nations,
                               std::vector<hex_sprite*>& fraction,
                               overworld::type_frac type,
                               std::vector<sf::Vector2i>& sprites);
    void setup_nations_from_save(std::vector<nation_info>& nations,
                                    std::vector<hex_sprite*>& fraction,
                                    overworld::type_frac type,
                                    std::vector<sf::Vector2i>& sprites);
    double get_nation_reputation_value(overworld::type_frac id) {return *reputation_pointers[id];}
    void set_battlezones_from_save(std::vector<hex_hold_>& grid,
                                    std::vector<nation_info>& nations,
                                    std::vector<battle_info::battlezone_info>& battlezones);
    bool is_game_over(std::vector<nation_info>& nations);
};

} // namespace overworld
} // namespace icarus
#endif // NATION_CREATOR_H
