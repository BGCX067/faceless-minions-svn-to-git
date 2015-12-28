#ifndef ICARUS_AI_SELECTOR_NODE_HPP_
#define ICARUS_AI_SELECTOR_NODE_HPP_

#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class selector_node : public behavior_node
{
private:
    bool random_;
    unsigned current_index_;
public:
    selector_node(bool random) : behavior_node(),
                                random_(random),
                                current_index_(0) {}
    virtual ~selector_node();
    virtual node_result execute(node_result prev_result);
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_SELECTOR_NODE_HPP_
