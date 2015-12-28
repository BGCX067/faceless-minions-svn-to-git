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

node_result selector_node::execute(node_result prev_result)
{
    node_result result;
    if (prev_result.down_)
    {
        if (child_nodes_.size() > 0)
        {
            if (random_)
            {
                unsigned index = floor(rand() % child_nodes_.size());
                result.next_ = child_nodes_[index];
                result.result_ = status::running;
                result.down_ = true;
                running_child_ = index;
            }
            else
            {
                result.next_ = child_nodes_[current_index_];
                result.result_ = status::running;
                result.down_ = true;
                running_child_ = current_index_;
                current_index_++;
            }
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
        if (running_child_ != -1 && prev_result.result_ == status::running)
        {
            result.next_ = child_nodes_[running_child_];
            result.result_ = status::running;
            result.down_ = true;
        }
        else if (prev_result.result_ == status::failed)
        {
            if (current_index_ < child_nodes_.size())
            {
                result.next_ = this;
                result.result_ = status::running;
                result.down_ = true;
                running_child_ = -1;
            }
            else
            {
                result.next_ = parent_node_;
                result.result_ = status::failed;
                result.down_ = false;
                current_index_ = 0;
                running_child_ = -1;
            }
        }
        else
        {
            result.next_ = parent_node_;
            result.result_ = prev_result.result_;
            result.down_ = false;
            running_child_ = -1;
            current_index_ = 0;
        }
    }
    return result;
}
}   //namespace ai
}   //namespace icarus
