#include <cstdlib>
#include <cmath>
#include <cstring>

#include "icarus/overworld/party_handler.hpp"
#include "icarus/math.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"

namespace icarus
{
namespace overworld
{
party_handler* party_handler::instance_ = NULL;

party_handler::party_handler()
:
    party_gold_(200)
{
    state_.state_ = utils::encounter_state::no_encounter;
    state_.region_ = 0;
    state_.encounter_ = 0;
    state_.selected_ = 0;
    state_.scenario_ = 0;
    state_.party_xp_ = 0;

    for (unsigned i = 0; i < 6; ++i)
    {
        party_[i] = NULL;
        utils::party_member temp;
        temp.active_ = 0;
        encounter_members_.push_back(temp);
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
            std::string gender = (bool(rand()%2) ? "male_first" : "female_first");
            utils::yth_node* name_node = resource_handler::get()->get_root_node("hero_names");
            int index_first = rand()%name_node->child(gender)->child_count("name");
            int index_last = rand()%name_node->child("surnames")->child_count("name");
            std::string name = name_node->child(gender)->child("name", index_first)->value();
            std::string surname = name_node->child("surnames")->child("name", index_last)->value();

            switch(int(rand()%3))
            {
            case 0: party_[i] = new party_member(i, name, surname, (gender == "male_first" ? 0 : 1), index_first, index_last, encounter::hero_class::warrior, level); break;
            case 1: party_[i] = new party_member(i, name, surname, (gender == "male_first" ? 0 : 1), index_first, index_last, encounter::hero_class::rogue, level); break;
            case 2: party_[i] = new party_member(i, name, surname, (gender == "male_first" ? 0 : 1), index_first, index_last, encounter::hero_class::mage, level); break;
            default: break;
            }
            if (gui::gui_handler::get()->party_list_is_visible())
                gui::gui_handler::get()->display_party_list();
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
            int level = math::clamp(int(1) + (level_diff_max < 0 ? -level_adjust : level_adjust), 1, 10);
            //TODO:: get names from YTH
            std::string gender = (bool(rand()%2) ? "male_first" : "female_first");
            utils::yth_node* name_node = resource_handler::get()->get_root_node("hero_names");
            int index_first = rand()%name_node->child(gender)->child_count("name");
            int index_last = rand()%name_node->child("surnames")->child_count("name");
            std::string name = name_node->child(gender)->child("name", index_first)->value();
            std::string surname = name_node->child("surnames")->child("name", index_last)->value();

            party_[i] = new party_member(i, name, surname, (gender == "male_first" ? 0 : 1), index_first, index_last, new_class, level);
            if (gui::gui_handler::get()->party_list_is_visible())
                gui::gui_handler::get()->display_party_list();
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
void party_handler::get_party_data(encounter::data* data_ptr, bool reset_metrics) const
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            encounter::hero_data data;
            party_[i]->generate_hero_data(data);
            if (reset_metrics)
            {
                data.kills_ = 0;
                data.assists_ = 0;
                data.damage_done_ = 0;
                data.damage_taken_ = 0;
                data.largest_given_ = 0;
                data.largest_taken_ = 0;
                data.friendly_fire_ = 0;
            }
            data_ptr->player_party_.push_back(data);
        }
    }

    // edit force multiplier
    data_ptr->force_multiplier_ = 1;
    //std::cout << level_mult << " " << hero_mult << " " << data_ptr->force_multiplier_ << std::endl;
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
        survivors += (data_ptr->player_party_[i].current_health_ > 0 ? 1 : 0);
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
                    graveyard_.push_back(party_[k]->get_name());
                    remove_member(k);
                    break;
                }
                party_[k]->add_xp(data_ptr->experience_value_/survivors);
            }
        }
    }
    if (gui::gui_handler::get()->party_list_is_visible())
        gui::gui_handler::get()->display_party_list();
}
bool party_handler::is_game_over() const
{
    return !(get_party_count());
}
unsigned party_handler::get_gold_amount()
{
    return party_gold_;
}
void party_handler::add_gold(unsigned amount)
{
    party_gold_ += amount;
}
void party_handler::subtract_gold(unsigned amount)
{
    party_gold_ -= amount;
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

bool party_handler::party_contains(encounter::hero_class::type type) const
{
    bool result = false;
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            result = party_[i]->get_class() == type ? true : result;
        }
    }
    return result;
}
unsigned party_handler::highest_level_of_class(encounter::hero_class::type type) const
{
    unsigned level = 0;
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL && party_[i]->get_class() == type &&
            party_[i]->get_level() > level)
        {
            level = party_[i]->get_level();
        }
    }
    return level;
}
bool party_handler::unspent_points() const
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL &&
            (party_[i]->get_attrib_pts() > 0 ||
             party_[i]->get_skill_pts() > 0))
        {
            return true;
        }
    }
    return false;
}
bool party_handler::swap_members(unsigned a, unsigned b)
{
    if (a >= 6 || b >= 6 ||
        (party_[a] == NULL && party_[b]== NULL))
        return false;
    party_member* temp = party_[a];
    party_[a] = party_[b];
    if (party_[a] != NULL)
        party_[a]->set_index(a);
    party_[b] = temp;
    if (party_[b] != NULL)
        party_[b]->set_index(b);
    return true;
}
void party_handler::apply_items()
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
            party_[i]->apply_all_items();
    }
}

void party_handler::save(std::ofstream& stream)
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i] != NULL)
        {
            party_[i]->save(stream);
        }
        else
        {
            utils::character member;
            std::memset(member.bytes, 0, sizeof(member.bytes));
            member.active_ = 0;
            stream.write(member.bytes, sizeof(member.bytes));
            for (unsigned k = 0; k < 6; ++k)
            {
                utils::item saved_item;
                std::memset(saved_item.bytes, 0, sizeof(saved_item.bytes));
                saved_item.active_ = 0;
                stream.write(saved_item.bytes, sizeof(saved_item.bytes));
            }
        }
    }
}
void party_handler::load(std::ifstream& stream)
{
    for (unsigned i = 0; i < 6; ++i)
    {
        utils::character member;
        std::memset(member.bytes, 0, sizeof(member.bytes));
        stream.read(member.bytes, sizeof(member.bytes));
        if (int(member.active_) == 1)
        {
            party_[i] = new party_member(i);
            party_[i]->load_character(member);
        }
        for (unsigned k = 0; k < 6; ++k)
        {
            utils::item saved_item;
            std::memset(saved_item.bytes, 0, sizeof(saved_item.bytes));
            stream.read(saved_item.bytes, sizeof(saved_item.bytes));
            if (party_[i] != NULL && int(saved_item.active_) == 1)
            {
                item* new_item = new item();
                new_item->load_item(saved_item);
                item* temp = NULL;
                party_[i]->swap_item(new_item, temp, (k > 3 ? k - 4 : k), bool(k > 3));
            }
        }
        if (party_[i] != NULL)
        {
            party_[i]->apply_all_items();
            party_[i]->set_current_hp_abs(unsigned(member.current_hp_));
        }
    }
}
}   // namespace overworld
}   // namespace icarus
