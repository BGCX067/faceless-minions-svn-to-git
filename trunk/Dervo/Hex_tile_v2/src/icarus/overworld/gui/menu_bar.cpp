#include "icarus/overworld/gui/menu_bar.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
#include "icarus/input_handler.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
menu_bar::menu_bar()
:
    party_button_(0x190701, 0x190701 + 0x333300),
    stats_button_(0x190701, 0x190701 + 0x333300),
    options_button_(0x190701, 0x190701 + 0x333300)
{
    utils::yth_node* menu_root = resource_handler::get()->get_root_node("ui")->child("overworld_menu");
    std::string tex_path = menu_root->child("background")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_ = sf::Sprite(*tex_ptr);

    tex_path = menu_root->child("button_graphic")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
    {
        party_button_.graphic_ = sf::Sprite(*tex_ptr);
        stats_button_.graphic_ = sf::Sprite(*tex_ptr);
        options_button_.graphic_ = sf::Sprite(*tex_ptr);
    }

    party_button_.text_.setString(menu_root->child("party_button")->value());
    party_button_.text_.setCharacterSize(16);
    party_button_.text_.setColor(utils::rgb(0x190701));
    party_button_.text_.setFont(*resource_handler::get()->get_font("text"));
    stats_button_.text_.setString(menu_root->child("stats_button")->value());
    stats_button_.text_.setCharacterSize(16);
    stats_button_.text_.setColor(utils::rgb(0x190701));
    stats_button_.text_.setFont(*resource_handler::get()->get_font("text"));
    options_button_.text_.setString(menu_root->child("menu_button")->value());
    options_button_.text_.setCharacterSize(16);
    options_button_.text_.setColor(utils::rgb(0x190701));
    options_button_.text_.setFont(*resource_handler::get()->get_font("text"));

    //todo:: placement
    reset_position();
}
menu_bar::~menu_bar()
{

}

void menu_bar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background_, states);
    target.draw(party_button_.graphic_, states);
    target.draw(party_button_.text_, states);
    target.draw(stats_button_.graphic_, states);
    target.draw(stats_button_.text_, states);
    target.draw(options_button_.graphic_, states);
    target.draw(options_button_.text_, states);
}
void menu_bar::handle_input(const std::vector<sf::Event>& input_events)
{
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch (input_events[i].type)
        {
        case sf::Event::MouseMoved:
        {
            math::vector2f mpos(input_events[i].mouseMove.x,
                                input_events[i].mouseMove.y);
            mpos = input_handler::get()->convert_mouse_pos(mpos);
            party_button_.set_hover(party_button_.contains(mpos));
            stats_button_.set_hover(stats_button_.contains(mpos));
            options_button_.set_hover(options_button_.contains(mpos));
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            math::vector2f mpos(input_events[i].mouseButton.x,
                                input_events[i].mouseButton.y);
            mpos = input_handler::get()->convert_mouse_pos(mpos);
            if (party_button_.contains(mpos))
            {
                gui_handler::get()->toggle_party_list();
            }
            else if (stats_button_.contains(mpos))
            {

            }
            else if (options_button_.contains(mpos))
            {

            }
            break;
        }
        default: break;
        }
    }
}
bool menu_bar::contains(const math::vector2f& mouse_pos) const
{
    return background_.getGlobalBounds().contains(mouse_pos);
}

void menu_bar::reset_position()
{
    background_.setPosition(input_handler::get()->convert_mouse_pos(math::vector2f(0.0f, 0.0f)));
    party_button_.set_position(input_handler::get()->convert_mouse_pos(math::vector2f(25.0f, 3.0f)));
    stats_button_.set_position(input_handler::get()->convert_mouse_pos(math::vector2f(225.0f, 3.0f)));
    options_button_.set_position(input_handler::get()->convert_mouse_pos(math::vector2f(425.0f, 3.0f)));
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
