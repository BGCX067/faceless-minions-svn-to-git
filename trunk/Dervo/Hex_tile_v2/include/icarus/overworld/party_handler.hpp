#ifndef ICARUS_OVERWORLD_PARTY_HANDLER_HPP_
#define ICARUS_OVERWORLD_PARTY_HANDLER_HPP_

#include "icarus/overworld/party_member.hpp"
#include "icarus/encounter/data.hpp"

namespace icarus
{
namespace overworld
{
class party_handler
{
private:
    party_member* party_[6];
    static party_handler* instance_;

    party_handler();
    party_handler(party_handler const&);
    void operator=(party_handler const&);

    unsigned party_gold;
public:
    virtual ~party_handler();
    static party_handler* get()
    {
        if (!instance_)
            instance_ = new party_handler();
        return instance_;
    }

    void add_party_member(int level_diff_max);
    void add_party_member(encounter::hero_class::type new_class,
                          int level_diff_max);
    party_member* get_member(unsigned index);
    void remove_member(unsigned index);
    void get_party_data(encounter::data* data_ptr) const;
    bool is_party_full() const;
    unsigned get_avg_level() const;
    unsigned get_party_count() const;
    void update_party(const encounter::data* const data_ptr);
    bool is_game_over() const;
    unsigned get_gold_amount();
    void add_gold(unsigned amount);
    void subtract_gold(unsigned amount);
    void heal_party(float amount);
};
}
}

#endif // ICARUS_OVERWORLD_PARTY_HANDLER_HPP_
