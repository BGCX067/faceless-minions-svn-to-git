#ifndef ICARUS_AI_SEQUENCE_NODE_HPP_
#define ICARUS_AI_SEQUENCE_NODE_HPP_

#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class sequence_node : public behavior_node
{
private:
    unsigned current_index_;

public:
    sequence_node() : behavior_node(), current_index_(0) {}
    virtual ~sequence_node();
    virtual status::value execute();
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_SEQUENCE_NODE_HPP_
