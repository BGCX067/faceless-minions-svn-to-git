#ifndef ICARUS_AI_FORCED_RESULT_NODE_HPP_
#define ICARUS_AI_FORCED_RESULT_NODE_HPP_

#include "icarus/ai/behavior_node.hpp"

namespace icarus
{
namespace ai
{
class forced_result_node : public behavior_node
{
private:
    status::value result_;
public:
    forced_result_node(status::value result) : behavior_node(),
                                            result_(result) {}
    virtual ~forced_result_node();
    virtual status::value execute();
};
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_FORCED_RESULT_NODE_HPP_


