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

status::value debug_node::execute()
{
    std::cout << message_ << std::endl;
    if (child_nodes_.size() > 0)
        return child_nodes_[0]->execute();
    return status::success;
}
}   //namespace ai
}   //namespace icarus
