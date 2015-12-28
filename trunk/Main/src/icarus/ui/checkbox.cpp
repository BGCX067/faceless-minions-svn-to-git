#include "icarus/ui/checkbox.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace ui
{
checkbox::checkbox()
:
    is_checked_(false),
    is_enabled_(true)
{
}
checkbox::~checkbox()
{
}

void checkbox::setup(utils::yth_node* data, bool initially_checked)
{
    utils::yth_node* sprite = resource_handler::get()->get_root_node("sprite_sheets")->child(data->child("sprite_sheet")->value());
    box_.load_sprite(sprite->child(data->child("sprite_name")->value()), sprite->child("path")->value());
    set_checked(initially_checked);
}

void checkbox::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!is_enabled_)
    {
        sf::Shader* bwshader = resource_handler::get()->get_shader("grayscale_luminosity");
        bwshader->setParameter("factor", 0.0f);
        bwshader->setParameter("texture", *box_.get_sprite()->getTexture());
        states.shader = bwshader;
    }
    target.draw(box_, states);
}
}   // namespace ui
}   // namespace icarus
