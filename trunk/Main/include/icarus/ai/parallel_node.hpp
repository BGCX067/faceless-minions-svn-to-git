#ifndef ICARUS_AI_PARALLEL_NODE_HPP_
#define ICARUS_AI_PARALLEL_NODE_HPP_

#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class parallel_node : public behavior_node
{
private:
    bool fail_on_first_;
    bool succeed_on_first_;
public:
    parallel_node(bool fail_on_first,
                  bool succeed_on_first) : behavior_node(),
                                           fail_on_first_(fail_on_first),
                                           succeed_on_first_(succeed_on_first) {}
    virtual ~parallel_node();
    virtual node_result execute(node_result prev_result);
    virtual void terminate();
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_PARALLEL_NODE_HPP_
