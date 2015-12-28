#include "icarus/ai/reversal_node.hpp"

namespace icarus
{
namespace ai
{
reversal_node::~reversal_node()
{
    if (child_nodes_.size() > 0 && child_nodes_[0])
    {
        child_nodes_[0]->terminate();
        delete child_nodes_[0];
    }
}

status::value reversal_node::execute()
{
    if (child_nodes_.size() > 0)
    {
        status::value return_status;
        switch (return_status = child_nodes_[0]->execute())
        {
        case status::success:
            return status::failed;
        case status::failed:
            return status::success;
        default:
            return return_status;
        }
    }
    return status::error;
}
}   //namespace ai
}   //namespace icarus
