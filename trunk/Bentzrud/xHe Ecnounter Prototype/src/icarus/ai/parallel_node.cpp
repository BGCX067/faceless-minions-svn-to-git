#include "icarus/ai/parallel_node.hpp"

namespace icarus
{
namespace ai
{
parallel_node::~parallel_node()
{
    for (unsigned i = 0; i < child_nodes_.size(); ++i)
    {
        child_nodes_[i]->terminate();
        if (child_nodes_[i]) delete child_nodes_[i];
    }
}

status::value parallel_node::execute()
{
    if (child_nodes_.size() == 0)
        return status::error;

    bool failed = true;
    bool succeeded = true;
    for (unsigned i = 0; i < child_nodes_.size(); ++i)
    {
        switch (child_nodes_[i]->execute())
        {
        case status::running:
            succeeded = false;
            failed = false;
            break;
        case status::success:
            if (succeed_on_first_)
                return status::success;
            else
                failed = false;
            break;
        case status::failed:
            if (fail_on_first_)
                return status::failed;
            else
                succeeded = false;
            break;
        case status::error:
            terminate();
            return status::error;
            break;
        }
    }
    if (succeeded)
        return status::success;
    else if (failed)
        return status::failed;
    else
        return status::running;
}

void parallel_node::terminate()
{
    for (unsigned i = 0; i < child_nodes_.size(); ++i)
        child_nodes_[i]->terminate();
}
}   // namespace ai
}   // namespace icarus
