#ifndef ENCOUNTER_CALCULATOR_H
#define ENCOUNTER_CALCULATOR_H
#include "icarus/resource_handler.hpp"
#include "hex_sprite.hpp"
#include "statistics.hpp"
namespace icarus
{
namespace overworld
{
enum type_tile {FORREST, HILL, LAKE, MOUNTAIN, SOCIAL,
        NATION1, NATION2, NATION3, NEUTRAL};
struct struct_list
{
    utilities::yth_node* encounter;
    int value;
};

class encounter_calculator
{
private:
    utilities::yth_node* encounter_list;
    //reference to hex_grid
protected:
public:
    encounter_calculator();
    void setup_data_link(utilities::yth_node* data);
    virtual ~encounter_calculator();
    void setup_encounter(hex_sprite* encounter_on);
};

} // namespace overworld
} // namespace icarus

#endif // ENCOUNTER_CALCULATOR_H
