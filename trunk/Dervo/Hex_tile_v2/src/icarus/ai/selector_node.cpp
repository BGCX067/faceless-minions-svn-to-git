#include <cstdlib>
#include <cmath>
#include "icarus/ai/selector_node.hpp"

namespace icarus
{
namespace ai
{
selector_node::~selector_node()
{
    for (unsigned i = 0; i < child_nodes_.size(); ++i)
    {
        child_nodes_[i]->terminate();
        if (child_nodes_[i]) delete child_nodes_[i];
    }
}

status::value selector_node::execute()
{
    if (child_nodes_.size() == 0 || child_nodes_.size() - 1 < current_index_)
        return status::error;

    if (random_)
    {
        unsigned index = (running_child_ != -1) ?
                         static_cast<unsigned>(running_child_) :
                         floor(rand() % child_nodes_.size());
        switch (child_nodes_[index]->execute())
        {
        case status::running:
            running_child_ = static_cast<int>(index);
            return status::running;
            break;
        case status::success:
            running_child_ = -1;
            return status::success;
            break;
        case status::failed:
            running_child_ = -1;
            return status::failed;
        case status::error:
            running_child_ = -1;
            return status::error;
            break;
        }
    }
    else
    {
        unsigned index = (running_child_ != -1) ?
                         static_cast<unsigned>(running_child_) :
                         current_index_;
        switch (child_nodes_[index]->execute())
        {
        case status::running:
            running_child_ = static_cast<int>(index);
            return status::running;
            break;
        case status::success:
            running_child_ = -1;
            current_index_ = 0;
            return status::success;
            break;
        case status::failed:
            current_index_++;
            running_child_ = -1;
            if (current_index_ >= child_nodes_.size())
            {
                current_index_ = 0;
                return status::failed;
            }
            return execute();
            break;
        case status::error:
            current_index_ = 0;
            running_child_ = -1;
            return status::error;
            break;
        }
    }
    return status::error;
}
}   //namespace ai
}   //namespace icarus
