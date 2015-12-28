#include "icarus/overworld/battlezone.hpp"
namespace icarus
{
namespace overworld
{
battlezone::battlezone() :
                stats(statistics::get()),
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
                            overworld::type_frac attacker,
                            overworld::type_frac defender,
                            unsigned delay)
{
    battle_point_ = battle_point;
    attacker_ = attacker;
    defender_ = defender;
    battle_is_active_ = true;
    round_count_ = delay;
    battle_point_->set_siege(true);
    battle_point_->set_attacking_nation(attacker);
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
            battle_point_->set_siege(false);
            battle_point_->set_attacking_nation(4);
            is_finished = true;
            attacker_won_ = true;
        }
        else
        {
            battle_point_->set_siege(false);
            battle_point_->set_attacking_nation(4);
            is_finished = true;
            attacker_won_ = false;
        }
    }
    round_count_++;
    return is_finished;
}
void battlezone::run_result(bool attacker_won)
{

}
bool battlezone::is_sucsessfull_fight(overworld::type_frac fraction)
{
    if((statistics::get()->battle_modifier + statistics::get()->army_size_per_nation[fraction]) < ((rand() % 10000) *0.0001) )
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
void battlezone::remove_siege()
{
    reset_battlezone();
    battle_point_->set_siege(false);
    battle_point_->set_attacking_nation(4);
    battle_point_ = 0;
}
} // namespace overworld
} // namespace icarus
