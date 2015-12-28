#include <iostream>
#include "icarus/ai/debug_node.hpp"

namespace icarus
{
namespace ai
{
debug_node::~debug_node()
{
    if (child_nodes_.size() > 0 && child_nodes_[0])
    {
        child_nodes_[0]->terminate();
        delete child_nodes_[0];
    }
}

node_result debug_node::execute(node_result prev_result)
{
    node_result result;
    if (prev_result.down_)
    {
        std::cout << message_ << std::endl;
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
        result.result_ = prev_result.result_;
        result.down_ = false;
    }
    return result;
}
}   //namespace ai
}   //namespace icarus
