#include <cstdlib>
#include <cmath>

#include "icarus/overworld/party_handler.hpp"
#include "icarus/math.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace overworld
{
party_handler* party_handler::instance_ = NULL;

party_handler::party_handler()
{
    for (unsigned i = 0; i < 6; ++i)
    {
        party_[i] = NULL;
    }
}
party_handler::~party_handler()
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            delete party_[i];
        }
    }
}

void party_handler::add_party_member(int level_diff_max)
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if(party_[i] == NULL)
        {
            int level_adjust = level_diff_max != 0 ? ceil(rand()%(abs(level_diff_max))) : 0;
            int level = math::clamp(int(get_avg_level()) + (level_diff_max < 0 ? -level_adjust : level_adjust), 0, 10);
            //TODO:: get names from YTH
            std::string sex = (bool(rand()%2) ? "male_first" : "female_first");
            utils::yth_node* name_node = resource_handler::get()->get_root_node("hero_names");
            int index_first = rand()%name_node->child(sex)->child_count("name");
            int index_last = rand()%name_node->child("surnames")->child_count("name");
            std::string name = name_node->child(sex)->child("name", index_first)->value();
            std::string surname = name_node->child("surnames")->child("name", index_last)->value();

            switch(int(rand()%3))
            {
            case 0: party_[i] = new party_member(i, name, surname, encounter::hero_class::warrior, level); break;
            case 1: party_[i] = new party_member(i, name, surname, encounter::hero_class::rogue, level); break;
            case 2: party_[i] = new party_member(i, name, surname, encounter::hero_class::mage, level); break;
            default: break;
            }
            return;
        }
    }
}
void party_handler::add_party_member(encounter::hero_class::type new_class,
                                     int level_diff_max)
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if(party_[i] == NULL)
        {
            int level_adjust = level_diff_max != 0 ? ceil(rand()%(abs(level_diff_max))) : 0;
            int level = math::clamp(int(get_avg_level()) + (level_diff_max < 0 ? -level_adjust : level_adjust), 0, 10);
            //TODO:: get names from YTH
            std::string sex = (bool(rand()%2) ? "male_first" : "female_first");
            utils::yth_node* name_node = resource_handler::get()->get_root_node("hero_names");
            int index_first = rand()%name_node->child(sex)->child_count("name");
            int index_last = rand()%name_node->child("surnames")->child_count("name");
            std::string name = name_node->child(sex)->child("name", index_first)->value();
            std::string surname = name_node->child("surnames")->child("name", index_last)->value();

            party_[i] = new party_member(i, name, surname, new_class, level); break;

            return;
        }
    }
}

party_member* party_handler::get_member(unsigned index)
{
    return party_[index];
}
void party_handler::remove_member(unsigned index)
{
    if (party_[index] != NULL)
    {
        delete party_[index];
        party_[index] = NULL;
    }
}
void party_handler::get_party_data(encounter::data* data_ptr) const
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            encounter::hero_data data;
            party_[i]->generate_hero_data(data);
            data_ptr->player_party_.push_back(data);
        }
    }
}
bool party_handler::is_party_full() const
{
    return (get_party_count() >= 6);
}
unsigned party_handler::get_avg_level() const
{
    unsigned total = 0;
    unsigned count = 0;
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            total += party_[i]->get_level();
            count++;
        }
    }
    return unsigned(count > 0 ? total/count : 1);
}
unsigned party_handler::get_party_count() const
{
    unsigned count = 0;
    for (unsigned i = 0; i < 6; ++i)
        if (party_[i] != NULL)
            count++;
    return count;
}
void party_handler::update_party(const encounter::data* const data_ptr)
{
    unsigned survivors = 0;
    for (unsigned i = 0; i < data_ptr->player_party_.size(); ++i)
        survivors += ceil(data_ptr->player_party_[i].current_health_);
    for (unsigned i = 0; i < data_ptr->player_party_.size(); ++i)
    {
        for (unsigned k = 0; k < 6; ++k)
        {
            if (party_[k] != NULL &&
                k == data_ptr->player_party_[i].index_)
            {
                party_[k]->update(data_ptr->player_party_[i]);
                if (party_[k]->is_dead())
                {
                    remove_member(k);
                    break;
                }
                party_[k]->add_xp(data_ptr->experience_value_/survivors);
            }
        }
    }
}
bool party_handler::is_game_over() const
{
    return !(get_party_count());
}
unsigned party_handler::get_gold_amount()
{
    return party_gold;
}
void party_handler::add_gold(unsigned amount)
{
    party_gold += amount;
}
void party_handler::subtract_gold(unsigned amount)
{
    party_gold -= amount;
}
void party_handler::heal_party(float amount)
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            party_[i]->heal(amount);
        }
    }
}
}
}
