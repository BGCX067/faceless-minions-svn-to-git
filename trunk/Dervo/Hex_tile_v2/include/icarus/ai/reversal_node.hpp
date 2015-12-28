#ifndef ICARUS_AI_REVERSAL_NODE_HPP_
#define ICARUS_AI_REVERSAL_NODE_HPP_

#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class reversal_node : public behavior_node
{
private:
public:
    reversal_node() :
        behavior_node() {}
    virtual ~reversal_node();
    virtual status::value execute();
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_REVERSAL_NODE_HPP_


