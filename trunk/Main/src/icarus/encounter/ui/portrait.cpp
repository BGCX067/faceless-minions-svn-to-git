#include "icarus/encounter/ui/portrait.hpp"
#include "icarus/utils.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
portrait::portrait(unsigned index,
                   const std::string& name,
                   const sf::Sprite& port)
:
    portrait_(port),
    cast_bar_(utils::rgb(0x3333DD), utils::rgb(0x220a02), ""),
    offset_x_(0),
    offset_y_(0),
    selected_(false)
{
    // todo:: implement frame
    // TODO:: implement portrait generation
    cast_bar_.set_label_position(label_position::v_center, label_position::h_right);
    health_.set_label_position(label_position::v_center, label_position::h_right);
    health_.set_label("");

    name_.setString(name);
    name_.setCharacterSize(14);
    name_.setFont(*resource_handler::get()->get_font("text"));

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
    offset_y_ = utils::to_hd(5);
    offset_x_ = utils::to_hd(index * 210 + 10);
    portrait_.setPosition(static_cast<float>(offset_x_),
                          static_cast<float>(offset_y_));
    offset_x_ += portrait_.getLocalBounds().width + 5.0f;
    health_.set_position(math::vector2f(static_cast<float>(offset_x_ + 60),
                                        static_cast<float>(offset_y_ + 20)));
    cast_bar_.set_position(math::vector2f(static_cast<float>(offset_x_ + 60),
                                          static_cast<float>(offset_y_ + 40)));
    name_.setPosition(math::vector2f(offset_x_, offset_y_) + math::vector2f(0.0f, 10.0f));
    for (unsigned i = 0; i < 5; ++i)
    {
        skill_icons_[i].setPosition(static_cast<float>(offset_x_ + (i * 20)),
                                    static_cast<float>(offset_y_ + 50));
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
    const sf::Shader* default_shader = states.shader;
    if (selected_)
    {
        resource_handler::get()->get_shader("outline")->setParameter("outline_color", utils::rgb(0xFFFF00));
        resource_handler::get()->get_shader("outline")->setParameter("outline_width", 3.0);
        resource_handler::get()->get_shader("outline")->setParameter("alpha", 0.7);
        resource_handler::get()->get_shader("outline")->setParameter("img_width", portrait_.getTexture()->getSize().x);
        resource_handler::get()->get_shader("outline")->setParameter("img_height", portrait_.getTexture()->getSize().y);
        resource_handler::get()->get_shader("outline")->setParameter("texture", *portrait_.getTexture());
        states.shader = resource_handler::get()->get_shader("outline");
    }
    target.draw(portrait_, states);
    states.shader = default_shader;
    //target.draw(frame_, states);
    health_.draw(target, states);
    cast_bar_.draw(target, states);
    for (unsigned i = 0; i < 5; ++i)
        target.draw(skill_icons_[i], states);
    target.draw(name_, states);
}

const sf::Texture* portrait::get_portrait_ptr()
{
    return portrait_.getTexture();
}
}   // namespace ui
}   // namespace encounter
}   // namespace icarus
