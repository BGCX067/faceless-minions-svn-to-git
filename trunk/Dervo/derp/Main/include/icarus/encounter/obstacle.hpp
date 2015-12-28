#ifndef ICARUS_ENCOUNTER_OBSTACLE_HPP_
#define ICARUS_ENCOUNTER_OBSTACLE_HPP_

#include "icarus/encounter/character_base.hpp"

namespace icarus
{
namespace encounter
{
class obstacle : public character_base
{
protected:

public:
    obstacle(grid* grid_ptr, const std::string& name);
    virtual ~obstacle();
    virtual bool apply_damage(int damage, uint8_t flags, character_base* source);
    void update(double delta);
};
}   // namespace encounter
}   // namespace icarus

#endif // ICARUS_ENCOUNTER_OBSTACLE_HPP_
