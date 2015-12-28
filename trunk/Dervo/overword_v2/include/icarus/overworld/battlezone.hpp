#ifndef BATTLEZONE_H
#define BATTLEZONE_H

#include "statistics.hpp"
#include "hex_sprite.hpp"

namespace icarus
{
namespace overworld
{
class battlezone
{
private:
    //hello
    statistics stats;

    hex_sprite* battle_point_;

    overworld::nation attacker_;
    overworld::nation defender_;

    unsigned round_count_;
    unsigned pre_battle_;

    bool attacker_won_;
    bool battle_is_active_;

protected:
public:
    battlezone();
    virtual ~battlezone();
    void set_battle(hex_sprite* battle_point,
                    overworld::nation attacker,
                    overworld::nation defender);
    bool run_battle();
    void run_result(bool attacker_won);
    bool is_sucsessfull_fight(overworld::nation fraction);
    bool return_winner();
    hex_sprite* return_siege_point();
    bool is_active();
    void reset_battlezone();

};
} // namespace overworld
} // namespace icarus

#endif // BATTLEZONE_H
