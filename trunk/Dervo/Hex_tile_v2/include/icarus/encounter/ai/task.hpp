
#ifndef ICARUS_ENCOUNTER_AI_TASK_HPP_
#define ICARUS_ENCOUNTER_AI_TASK_HPP_

#include "icarus/ai/behavior_node.hpp"
#include "icarus/encounter/character_npc.hpp"

namespace icarus
{
namespace ai
{
namespace encounter
{
class task : public behavior_node
{
private:
    typedef status::value (icarus::encounter::character_npc::*task_function)();
    task_function function_;
    icarus::encounter::character_npc& class_;
public:
    task(icarus::encounter::character_npc& class_ref,
              task_function func) : behavior_node(),
                                         function_(func),
                                         class_(class_ref) {}
    virtual ~task();
    virtual status::value execute();
};
}   //namespace encounter
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_ENCOUNTER_AI_TASK_HPP_


