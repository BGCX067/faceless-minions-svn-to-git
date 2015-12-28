#include "icarus/encounter/composite_skill.hpp"

namespace icarus
{
namespace encounter
{
composite_skill::composite_skill(std::string name,
                                 float cast_time,
                                 float cooldown,
                                 int range)
:
    name_(name),
    cast_time_(cast_time),
    cooldown_(cooldown),
    current_cooldown_(0.0f),
    range_(range)
{
    needs_target_ = static_cast<bool>(range_ > 0);
}

composite_skill::~composite_skill()
{
}

void composite_skill::add_subskill(targeted_skill subskill)
{
    subskills_.push_back(subskill);
}
void composite_skill::add_effect(applied_effect effect)
{
    if (subskills_.size() != 0)
        subskills_.back().effect = effect;
}

void composite_skill::update_cooldown(float delta)
{
    if (current_cooldown_ > 0.0f)
        current_cooldown_ -= delta;
}

//void composite_skill::get_subskills(std::vector<targeted_skill>& subskills) const
//{
//    for (unsigned i = 0; i < subskills_.size(); ++i)
//    {
//        subskills.push_back(subskills_[i]);
//    }
//}
}   // namespace encounter
}   // namespace icarus
