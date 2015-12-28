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
    visible_(false),
    drag_party_member_(false),
    start_drag_(false),
    dragged_index_(0),
    pos_t_(0.0f),
    movement_(0.0f)
{
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("party_list");
    std::string tex_path = dialog_root->child("background")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_ = sf::Sprite(*tex_ptr);

    up_pos_ = math::vector2f(atof(dialog_root->child("x_pos")->value().c_str()), -background_.getLocalBounds().height);
    down_pos_ = math::vector2f(atof(dialog_root->child("x_pos")->value().c_str()), atof(dialog_root->child("y_pos")->value().c_str()));
    current_pos_ = up_pos_;
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
    std::string class_name = "";
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
                class_name = "warrior";
                break;
            case encounter::hero_class::mage:
                class_level = "Level " + utils::to_str(member->get_level()) +
                                          " Alchemist";
                class_name = "mage";
                break;
            case encounter::hero_class::rogue:
                class_level = "Level " + utils::to_str(member->get_level()) +
                                          " Quicksilver";
                class_name = "rogue";
                break;
            }
            party_[i].set_up(name,
                             class_level,
                             member->get_current_hp(),
                             float(member->get_current_xp()) / float(member->get_xp_tnl()),
                             class_name, i);
        }
        else
        {
            party_[i].set_visible(false);
        }
    }
    visible_ = true;
    active_ = false;
    movement_ = 2.0f;
}
void party_list::hide_dialog()
{
    active_ = false;
    movement_ = -2.0f;
    gui_handler::get()->close_party_screen();
}
void party_list::handle_input(const std::vector<sf::Event>& input_events)
{
    if (!active_)
    {
        pos_t_ += movement_ * state_manager::get()->get_delta();
        pos_t_ = math::clamp(pos_t_, 0.0f, 1.0f);
        current_pos_ = math::v2lerp(up_pos_, down_pos_, pos_t_);
        reset_position();
        if (math::compare(pos_t_, 1.0f) || math::compare(pos_t_, 0.0f))
        {
            movement_ = 0.0f;
            if (math::compare(pos_t_, 0.0f))
            {
                active_ = false;
                visible_ = false;
            }
            else
            {
                active_ = true;
            }
        }
        return;
    }
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch (input_events[i].type)
        {
        case sf::Event::MouseButtonReleased:
            {
                math::vector2f mouse_pos(input_events[i].mouseButton.x,
                                         input_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);
                start_drag_ = false;
                for (unsigned k = 0; k < 6; ++k)
                {
                    if (party_[k].contains(mouse_pos))
                    {
                        if (party_[k].is_visible() && !drag_party_member_)
                        {
                            gui_handler::get()->display_party_screen(k);
                            break;
                        }
                        else if (drag_party_member_ && dragged_index_ != k)
                        {
                            swap_members(dragged_index_, k);
                            break;
                        }
                    }
                }
                if (drag_party_member_)
                {
                    drag_party_member_ = false;
                    reset_position();
                }
                break;
            }
        case sf::Event::MouseButtonPressed:
            {
                math::vector2f mouse_pos(input_events[i].mouseButton.x,
                                         input_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);
                for (unsigned k = 0; k < 6; ++k)
                {
                    if (party_[k].contains(mouse_pos))
                    {
                        if (party_[k].is_visible() && !drag_party_member_)
                        {
                            dragged_index_ = k;
                            start_drag_ = true;
                        }
                        break;
                    }
                }
                break;
            }
        case sf::Event::MouseMoved:
            {
                math::vector2f mouse_pos(input_events[i].mouseMove.x,
                                         input_events[i].mouseMove.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);

                if (start_drag_)
                    drag_party_member_ = true;

                if (drag_party_member_)
                {
                    party_[dragged_index_].set_position(mouse_pos);
                }
                break;
            }
        default: break;
        }
    }
}
void party_list::reset_position()
{
    math::vector2f origin(input_handler::get()->convert_mouse_pos(math::vector2f(0.0f, 0.0f)));
    utils::yth_node* node = resource_handler::get()->get_root_node("ui")->child("party_list");
    background_.setPosition(origin + current_pos_);
    math::vector2f pos = math::vector2f(background_.getPosition()) + math::vector2f(atof(node->child("member_offset_x")->value().c_str()),
                                                                                    atof(node->child("member_offset_y")->value().c_str()));
    float spacing = atof(node->child("member_spacing")->value().c_str());
    for (unsigned i = 0; i < 6; ++i)
    {
        if (!drag_party_member_ || (drag_party_member_ && dragged_index_ != i))
            party_[i].set_position(math::vector2f(pos.x, pos.y + (spacing * i)));
    }
}

bool party_list::swap_members(unsigned a, unsigned b)
{
    // hack.
    if (party_handler::get()->swap_members(a, b))
    {
        drag_party_member_ = false;
        setup_dialog();
        reset_position();
        return true;
    }
    else
    {
        drag_party_member_ = false;
        reset_position();
        return false;
    }
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
