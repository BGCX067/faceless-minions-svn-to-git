#include "icarus/overworld/gui/icon.hpp"
#include "icarus/utils.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
icon::icon(bool is_item)
:
    active_(is_item),
    highlight_(false),
    visible_(true)
{
    if (is_item)
    {
        tooltip_type_ = icarus::ui::tooltip_type::item;
    }
    else
    {
        tooltip_type_ = icarus::ui::tooltip_type::generic;
    }
}
icon::~icon()
{
}

void icon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_)
        return;
    if (highlight_)
    {
        sf::Shader* glowshader = resource_handler::get()->get_shader("faded_outline");
        glowshader->setParameter("texture", *sprite_.getTexture());
        glowshader->setParameter("img_width", sprite_.getLocalBounds().width);
        glowshader->setParameter("img_height", sprite_.getLocalBounds().height);
        glowshader->setParameter("outline_color", utils::rgb(0xFFFF00));
        glowshader->setParameter("outline_width", 5.0);
        states.shader = glowshader;
    }
    else if (!active_)
    {
        sf::Shader* grayshader = resource_handler::get()->get_shader("grayscale_luminosity");
        grayshader->setParameter("texture", *sprite_.getTexture());
        grayshader->setParameter("factor", 0.0);
        states.shader = grayshader;
    }
    target.draw(sprite_, states);
}
void icon::set_texture(const sf::Texture& tex)
{
    sprite_.setTexture(tex);
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
