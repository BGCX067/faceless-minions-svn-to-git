#ifndef ICARUS_AI_LOOP_NODE_HPP_
#define ICARUS_AI_LOOP_NODE_HPP_

#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class loop_node : public behavior_node
{
private:
    unsigned loops_;
    status::value desired_result_;
public:
    loop_node(unsigned loops, status::value desired_result) :
        behavior_node(),
        loops_(loops),
        desired_result_(desired_result) {}
    virtual ~loop_node();
    virtual status::value execute();
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_LOOP_NODE_HPP_


