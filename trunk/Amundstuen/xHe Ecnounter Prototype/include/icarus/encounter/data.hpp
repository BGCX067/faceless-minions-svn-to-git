#ifndef ICARUS_ENCOUNTER_DATA_HPP_
#define ICARUS_ENCOUNTER_DATA_HPP_

#include "icarus/state_data.hpp"

namespace icarus
{
namespace encounter
{
class data : public state_data
{
protected:
    scenario::type scenario_type_;

public:
    data(scenario::type scenario = scenario::undefined)
    :   scenario_type_(scenario) {}
    ~data() {}
    scenario::type get_scenario() const { return scenario_type_; }
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_DATA_HPP_
