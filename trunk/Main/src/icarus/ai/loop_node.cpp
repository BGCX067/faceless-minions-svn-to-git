#include "icarus/ai/loop_node.hpp"

namespace icarus
{
namespace ai
{
loop_node::~loop_node()
{
    if (child_nodes_.size() > 0 && child_nodes_[0])
    {
        child_nodes_[0]->terminate();
        delete child_nodes_[0];
    }
}

node_result loop_node::execute(node_result prev_result)
{
    node_result result;
    if (prev_result.down_)
    {
        if (child_nodes_.size() > 0)
        {
            result.next_ = child_nodes_[0];
            result.result_ = status::running;
            result.down_ = true;
            loop_count_++;
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
        if (loop_count_ < loops_ &&
            prev_result.result_ != desired_result_)
        {
            result.next_ = child_nodes_[0];
            result.result_ = status::running;
            result.down_ = true;
            loop_count_++;
        }
        else
        {
            loop_count_ = 0;
            result.next_ = parent_node_;
            result.result_ = prev_result.result_;
            result.down_ = false;
        }
    }
    return result;
}
}   //namespace ai
}   //namespace icarus
