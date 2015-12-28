#include "icarus/overworld/battlezone.hpp"
namespace icarus
{
namespace overworld
{
battlezone::battlezone() :
                round_count_(0),
                pre_battle_(3)
{
    //ctor
}

battlezone::~battlezone()
{
    //dtor
}
void battlezone::set_battle(hex_sprite* battle_point,
                            overworld::nation attacker,
                            overworld::nation defender)
{
    battle_point_ = battle_point;
    attacker_ = attacker;
    defender_ = defender;
    battle_is_active_ = true;
}
bool battlezone::run_battle()
{
    bool is_finished = false;
    //Hello!
    if(round_count_ >= pre_battle_)
    {
        //actuall battle
        bool attacker = is_sucsessfull_fight(attacker_);
        bool defender = is_sucsessfull_fight(defender_);

        if(attacker == defender)
        {
            // stalemate
        }
        else if(attacker == true)
        {
            is_finished = true;
            attacker_won_ = true;
            //run_result(attacker);
        }
        else
        {
            is_finished = true;
            attacker_won_ = false;
            //run_result(defender);
        }
    }
    round_count_++;
    return is_finished;
}
void battlezone::run_result(bool attacker_won)
{

}
bool battlezone::is_sucsessfull_fight(overworld::nation fraction)
{
    if((stats.battle_modifier + stats.army_size_per_nation[fraction]) < ((rand() % 10000) *0.0001) )
    {
        return true;
    }
    return false;
}
bool battlezone::return_winner()
{
    return attacker_won_;
}
hex_sprite* battlezone::return_siege_point()
{
    return battle_point_;
}
bool battlezone::is_active()
{
    return battle_is_active_;
}
void battlezone::reset_battlezone()
{
    battle_is_active_ = false;
}
} // namespace overworld
} // namespace icarus
