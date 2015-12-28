#ifndef ICARUS_AI_BEHAVIOR_NODE_HPP_
#define ICARUS_AI_BEHAVIOR_NODE_HPP_

#include <vector>
#include "icarus/ai.hpp"

namespace icarus
{
namespace ai
{
class behavior_node
{
protected:
    int running_child_;
    behavior_node* parent_node_;
    std::vector<behavior_node*> child_nodes_;

public:
    behavior_node() : running_child_(-1) {}
    virtual ~behavior_node() = 0;
    virtual status::value execute() = 0;

    virtual void terminate()
    {
        if (running_child_ != -1)
            child_nodes_[running_child_]->terminate();
    }

    virtual void add_child(behavior_node* child_node)
    { child_nodes_.push_back(child_node); }

    behavior_node* add(behavior_node* new_node)
    {
        add_child(new_node);
        new_node->set_parent(this);
        return new_node;
    }
    behavior_node* end() { return parent_node_; }
    void set_parent(behavior_node* parent) { parent_node_ = parent; }
};
inline behavior_node::~behavior_node() {}
}   //namespace ai
}   //namespace icarus

#endif // ICARUS_AI_BEHAVIOR_NODE_HPP_
