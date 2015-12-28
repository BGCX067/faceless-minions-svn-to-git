#include <iostream>
#include "icarus/ai/timer_node.hpp"

namespace icarus
{
namespace ai
{
timer_node::~timer_node()
{
    if (child_nodes_.size() > 0 && child_nodes_[0])
    {
        child_nodes_[0]->terminate();
        delete child_nodes_[0];
    }
}

node_result timer_node::execute(node_result prev_result)
{
    node_result result;
    if (prev_result.down_ && timer_.getElapsedTime().asSeconds() >= seconds_)
    {
        timer_.restart();
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
    else if (prev_result.down_)
    {
        result.next_ = parent_node_;
        result.result_ = status::failed;
        result.down_ = false;
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
