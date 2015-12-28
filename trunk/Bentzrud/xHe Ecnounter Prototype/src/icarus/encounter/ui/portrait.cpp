#include "icarus/encounter/ui/portrait.hpp"
#include "icarus/utils.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
portrait::portrait(unsigned index, const sf::Sprite& port)
:
    portrait_(port),
    cast_bar_(utils::rgb(0x3333DD), utils::rgb(0x333333), ""),
    offset_x_(0),
    offset_y_(0)
{
    // todo:: implement frame
    // TODO:: implement portrait generation
    cast_bar_.set_label_position(label_position::v_center, label_position::h_right);
    health_.set_label_position(label_position::v_center, label_position::h_right);
    health_.set_label("");

    for (unsigned i = 0; i < 5; ++i)
    {
        icon_states_[i] = false;
        skill_icons_[i].setPointCount(6);
        skill_icons_[i].setRadius(5.0f);
        set_skill_ready(i, true);
    }

    set_position(index);
}
portrait::~portrait()
{

}
void portrait::set_position(unsigned index)
{
    // TODO:: fewer magic numbers?
    offset_y_ = 15;
    offset_x_ = index * 220 + 40;
    portrait_.setPosition(static_cast<float>(offset_x_),
                          static_cast<float>(offset_y_));
    health_.set_position(math::vector2f(static_cast<float>(offset_x_ + 140),
                                        static_cast<float>(offset_y_ + 10)));
    cast_bar_.set_position(math::vector2f(static_cast<float>(offset_x_ + 140),
                                          static_cast<float>(offset_y_ + 30)));
    for (unsigned i = 0; i < 5; ++i)
    {
        skill_icons_[i].setPosition(static_cast<float>(offset_x_ + 80 + (i * 20)),
                                    static_cast<float>(offset_y_ + 40));
    }
}

void portrait::set_skill_ready(unsigned index, bool ready)
{
    if (ready && icon_states_[index] != ready)
    {
        skill_icons_[index].setFillColor(utils::rgb(0x00CC00));
        icon_states_[index] = true;
    }
    else if (icon_states_[index] != ready)
    {
        skill_icons_[index].setFillColor(utils::rgb(0xCC0000));
        icon_states_[index] = false;
    }
}

void portrait::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(portrait_, states);
    //target.draw(frame_, states);
    health_.draw(target, states);
    cast_bar_.draw(target, states);
    for (unsigned i = 0; i < 5; ++i)
        target.draw(skill_icons_[i], states);
}
}   // namespace ui
}   // namespace encounter
}   // namespace icarus
