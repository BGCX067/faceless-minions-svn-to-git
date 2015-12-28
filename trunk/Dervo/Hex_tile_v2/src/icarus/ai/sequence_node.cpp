#include "icarus/ai/sequence_node.hpp"

namespace icarus
{
namespace ai
{
sequence_node::~sequence_node()
{
    for (unsigned i = 0; i < child_nodes_.size(); ++i)
    {
        child_nodes_[i]->terminate();
        if (child_nodes_[i]) delete child_nodes_[i];
    }
}

status::value sequence_node::execute()
{
    if (child_nodes_.size() == 0 || child_nodes_.size() - 1 < current_index_)
        return status::error;

    switch (child_nodes_[current_index_]->execute())
    {
    case status::running:
        running_child_ = static_cast<int>(current_index_);
        return status::running;
        break;
    case status::success:
        current_index_++;
        running_child_ = -1;
        if (current_index_ >= child_nodes_.size())
        {
            current_index_ = 0;
            return status::success;
        }
        return execute();
        break;
    case status::failed:
        current_index_ = 0;
        running_child_ = -1;
        return status::failed;
        break;
    case status::error:
        current_index_ = 0;
        running_child_ = -1;
        return status::error;
        break;
    }
    return status::error;
}
}   // namespace ai
}   // namespace icarus
