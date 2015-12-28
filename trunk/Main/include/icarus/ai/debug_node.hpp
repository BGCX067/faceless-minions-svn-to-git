#ifndef ICARUS_AI_DEBUG_NODE_HPP_
#define ICARUS_AI_DEBUG_NODE_HPP_

#include <string>
#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class debug_node : public behavior_node
{
private:
    std::string message_;
public:
    debug_node(const std::string& message) : behavior_node(),
                                            message_(message) {}
    virtual ~debug_node();
    virtual node_result execute(node_result prev_result);
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_DEBUG_NODE_HPP_

