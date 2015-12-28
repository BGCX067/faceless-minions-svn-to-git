#include "icarus/overworld/gui/party_list.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
party_list::party_list()
:
    visible_(false)
{
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("party_list");
    std::string tex_path = dialog_root->child("background")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_ = sf::Sprite(*tex_ptr);
}
party_list::~party_list()
{
}

void party_list::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_)
        return;
    target.draw(background_, states);
    for (unsigned i = 0; i < 6; ++i)
        target.draw(party_[i], states);
}
void party_list::setup_dialog()
{
    overworld::party_member* member = NULL;
    std::string class_level = "";
    std::string name = "";
    for (unsigned i = 0; i < 6; ++i)
    {
        if ((member = party_handler::get()->get_member(i)) != NULL)
        {
            name = member->get_name(16);
            switch (member->get_class())
            {
            case encounter::hero_class::warrior:
                class_level = "Level " + utils::to_str(member->get_level()) +
                                          " Ironclad";
                break;
            case encounter::hero_class::mage:
                class_level = "Level " + utils::to_str(member->get_level()) +
                                          " Alchemist";
                break;
            case encounter::hero_class::rogue:
                class_level = "Level " + utils::to_str(member->get_level()) +
                                          " Quicksilver";
                break;
            }
            party_[i].set_up(name,
                             class_level,
                             member->get_current_hp(),
                             float(member->get_current_xp()) / float(member->get_xp_tnl()),
                             (member->get_attrib_pts() ||
                              member->get_skill_pts()));
        }
        else
        {
            party_[i].set_visible(false);
        }
    }
    visible_ = true;
}
void party_list::hide_dialog()
{
    visible_ = false;
    gui_handler::get()->close_party_screen();
}
void party_list::handle_input(const std::vector<sf::Event>& input_events)
{
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch (input_events[i].type)
        {
        case sf::Event::MouseButtonReleased:
            {
                math::vector2f mouse_pos(input_events[i].mouseButton.x,
                                         input_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);
                for (unsigned k = 0; k < 6; ++k)
                {
                    if (party_[k].is_visible() && party_[k].contains(mouse_pos))
                    {
                        gui_handler::get()->display_party_screen(k);
                        break;
                    }
                }
                break;
            }
        default: break;
        }
    }
}
void party_list::reset_position()
{
    utils::yth_node* node = resource_handler::get()->get_root_node("ui")->child("party_list");
    background_.setPosition(input_handler::get()->convert_mouse_pos(math::vector2f(atof(node->child("x_pos")->value().c_str()),
                                                                                   atof(node->child("y_pos")->value().c_str()))));
    math::vector2f pos = math::vector2f(background_.getPosition()) + math::vector2f(atof(node->child("member_offset_x")->value().c_str()),
                                                                                    atof(node->child("member_offset_y")->value().c_str()));
    float spacing = atof(node->child("member_spacing")->value().c_str());
    for (unsigned i = 0; i < 6; ++i)
    {
        party_[i].set_position(math::vector2f(pos.x, pos.y + (spacing * i)));
    }
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
