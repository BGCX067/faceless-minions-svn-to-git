#ifndef ENCOUNTER_CALCULATOR_H
#define ENCOUNTER_CALCULATOR_H
#include "icarus/resource_handler.hpp"
#include "hex_sprite.hpp"
#include "statistics.hpp"
#include "icarus/overworld/nation_creator.hpp"
namespace icarus
{
namespace overworld
{
struct struct_list
{
    utils::yth_node* encounter;
    int value;
};

class encounter_calculator
{
private:
    utils::yth_node* encounter_list;
    utils::yth_node* reference_to;
    statistics stats;
    //reference to hex_grid
protected:
public:
    encounter_calculator();
    void setup_data_link(utils::yth_node* data, utils::yth_node* target_reference);
    virtual ~encounter_calculator();
    void add_encounter_to_list(unsigned per_data, struct_list push_into,
                               unsigned& current_value,
                               std::vector<struct_list>& possible_encounters);
    void setup_encounter(hex_sprite* encounter_on, nation_creator& nations);
};

} // namespace overworld
} // namespace icarus

#endif // ENCOUNTER_CALCULATOR_H
