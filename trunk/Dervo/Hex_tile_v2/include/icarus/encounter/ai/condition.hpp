#ifndef ICARUS_ENCOUNTER_AI_CONDITION_HPP_
#define ICARUS_ENCOUNTER_AI_CONDITION_HPP_

#include "icarus/ai/behavior_node.hpp"
#include "icarus/encounter/character_npc.hpp"

namespace icarus
{
namespace ai
{
namespace encounter
{
class condition : public behavior_node
{
private:
    typedef bool (icarus::encounter::character_npc::*condition_function)();
    condition_function function_;
    icarus::encounter::character_npc& class_;
public:
    condition(icarus::encounter::character_npc& class_ref,
              condition_function func) : behavior_node(),
                                         function_(func),
                                         class_(class_ref) {}
    virtual ~condition();
    virtual status::value execute();
};
}   //namespace encounter
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_ENCOUNTER_AI_CONDITION_HPP_


