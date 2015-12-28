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

status::value loop_node::execute()
{
    if (child_nodes_.size() > 0)
    {
        for (unsigned i = 0; i < loops_; ++i)
        {
            if (child_nodes_[0]->execute() == desired_result_)
                return desired_result_;
        }
        return status::failed;
    }
    return status::error;
}
}   //namespace ai
}   //namespace icarus
