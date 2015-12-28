#include "icarus/overworld/gui/menu_bar.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/overworld/party_handler.hpp"

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
    options_button_(0x190701, 0x190701 + 0x333300),
    gold_count_(0),
    gold_interp_(false)
{
    utils::yth_node* menu_root = resource_handler::get()->get_root_node("ui")->child("overworld_menu");
    std::string tex_path = menu_root->child("background_left")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_left_ = sf::Sprite(*tex_ptr);
    tex_path = menu_root->child("background_right")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_right_ = sf::Sprite(*tex_ptr);

    tex_path = menu_root->child("gold_icon")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        gold_icon_ = sf::Sprite(*tex_ptr);

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

    party_gold_.setCharacterSize(22);
    party_gold_.setColor(utils::rgb(0xffdd4e));
    party_gold_.setFont(*resource_handler::get()->get_font("numbers"));

    //todo:: placement
    update_gold();
    reset_position();
}
menu_bar::~menu_bar()
{

}

void menu_bar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background_left_, states);
    target.draw(background_right_, states);
    target.draw(stats_button_.graphic_, states);
    target.draw(stats_button_.text_, states);
    target.draw(options_button_.graphic_, states);
    target.draw(options_button_.text_, states);
    sf::Shader* glowshader = resource_handler::get()->get_shader("faded_outline");
    const sf::Shader* defaultshader = states.shader;
    if (party_handler::get()->unspent_points())
    {
        glowshader->setParameter("texture", *party_button_.graphic_.getTexture());
        glowshader->setParameter("img_width", party_button_.graphic_.getLocalBounds().width);
        glowshader->setParameter("img_height", party_button_.graphic_.getLocalBounds().height);
        glowshader->setParameter("outline_color", utils::rgb(0xFFFF00));
        double x = double(state_manager::get()->get_milliseconds())/200.0;
        double width = abs(sin(x) * 5);
        glowshader->setParameter("outline_width", width);
        states.shader = glowshader;
    }
    target.draw(party_button_.graphic_, states);
    states.shader = defaultshader;
    target.draw(party_button_.text_, states);
    target.draw(party_gold_, states);
    target.draw(gold_icon_, states);
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
                gui_handler::get()->toggle_inventory();
            }
            else if (options_button_.contains(mpos))
            {
                state_manager::get()->push_state(states::options);
            }
            break;
        }
        default: break;
        }
    }
    update_gold();
}
bool menu_bar::contains(const math::vector2f& mouse_pos) const
{
    return background_left_.getGlobalBounds().contains(mouse_pos);
}

void menu_bar::reset_position()
{
    math::vector2f origin(input_handler::get()->convert_mouse_pos(math::vector2f(0.0f, 0.0f)));
    background_left_.setPosition(origin);
    party_button_.set_position(origin + math::vector2f(25.0f, 0.0f));
    stats_button_.set_position(origin + math::vector2f(225.0f, 0.0f));
    options_button_.set_position(origin + math::vector2f(425.0f, 0.0f));
    background_right_.setPosition(origin + (math::vector2f(state_manager::get()->get_dimensions().x - background_right_.getLocalBounds().width, 0.0f)));
    party_gold_.setPosition(origin + (math::vector2f(state_manager::get()->get_dimensions().x - party_gold_.getLocalBounds().width - 40.0f, 3.0f)));
    gold_icon_.setPosition(math::vector2f(party_gold_.getPosition().x - gold_icon_.getLocalBounds().width - 10.0f, origin.y + 3.0f));
}
void menu_bar::update_gold()
{
    int gold = party_handler::get()->get_gold_amount();
    if (static_cast<int>(gold_count_) != gold)
    {
        party_gold_.setColor((gold_count_ < gold ? utils::rgb(0x22EE22) : utils::rgb(0xEE2222)));
        gold_count_ += (gold_count_ < gold ? 100 : -100) * state_manager::get()->get_delta();
        party_gold_.setString(utils::to_str(static_cast<int>(gold_count_), 9));
    }
    else
    {
        party_gold_.setColor(utils::rgb(0xffdd4e));
    }
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
