#include "icarus/ui/base_tooltip.hpp"

namespace icarus
{
namespace ui
{
std::vector<base_tooltip*> base_tooltip::master_list_;
base_tooltip::base_tooltip()
:
    tooltip_type_(tooltip_type::generic),
    tooltip_data_(NULL),
    enabled_(false)
{
    master_list_.push_back(this);
}
base_tooltip::~base_tooltip()
{
    for (unsigned i = 0; i < master_list_.size(); ++i)
        if (master_list_[i] == this)
            master_list_.erase(master_list_.begin()+i);
}
}   // namespace ui
}   // namespace icarus

