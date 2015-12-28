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

status::value forced_result_node::execute()
{
    if (child_nodes_.size() > 0)
        if (child_nodes_[0]->execute() == status::error)
            return status::error;
    return result_;
}
}   //namespace ai
}   //namespace icarus
