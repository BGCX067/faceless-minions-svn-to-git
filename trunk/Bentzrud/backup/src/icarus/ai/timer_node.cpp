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

status::value timer_node::execute()
{
    if (timer_.getElapsedTime().asSeconds() >= seconds_)
    {
        timer_.restart();
        if (child_nodes_.size() > 0)
            return child_nodes_[0]->execute();
        return status::success;
    }
    return status::failed;
}
}   //namespace ai
}   //namespace icarus
