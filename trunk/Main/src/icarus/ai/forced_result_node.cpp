#include "icarus/ai/forced_result_node.hpp"

namespace icarus
{
namespace ai
{
forced_result_node::~forced_result_node()
{
    if (child_nodes_.size() > 0 && child_nodes_[0])
    {
        child_nodes_[0]->terminate();
        delete child_nodes_[0];
    }
}

node_result forced_result_node::execute(node_result prev_result)
{
    node_result result;
    if (prev_result.down_)
    {
        if (child_nodes_.size() > 0)
        {
            result.next_ = child_nodes_[0];
            result.result_ = status::running;
            result.down_ = true;
        }
        else
        {
            result.next_ = parent_node_;
            result.result_ = status::success;
            result.down_ = false;
        }
    }
    else
    {
        result.next_ = parent_node_;
        result.result_ = result_;
        result.down_ = false;
    }
    return result;
}
}   //namespace ai
}   //namespace icarus
