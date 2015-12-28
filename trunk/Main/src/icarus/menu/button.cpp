#include "icarus/menu/button.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/menu/menu_state.hpp"
#include "icarus/overworld/overworld_state.hpp"
#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace menu
{
button::button(const std::string& button_type)
:
    button_(NULL),
    target_(button_target::undefined),
    hover_(false),
    enabled_(true)
{
    utils::yth_node* ui_root = resource_handler::get()->get_root_node("ui");
    std::string button_path = ui_root->child("menu_buttons")->child(button_type)->child("sprite")->value();
    if (sf::Texture* button_tex = resource_handler::get()->get_texture(button_path))
        button_ = new sf::Sprite(*button_tex);
    label_.setString(ui_root->child("menu_buttons")->child(button_type)->child("label")->value());
    label_.setCharacterSize(30);
    label_.setFont(*resource_handler::get()->get_font("text"));
    label_.setColor(utils::rgb(!enabled_ ? utils::rgb(0xBBBBBB) : utils::rgb(0xffdd4e)));
}
button::~button()
{
    if (button_ != NULL)
        delete button_;
}

void button::click(sf::RenderWindow* window)
{
    switch(target_)
    {
    case button_target::undefined:
        break;
    case button_target::intro:
        break;
    case button_target::menu:
        state_manager::get()->change_state(states::menu);
        break;
    case button_target::new_game:
        state_manager::get()->set_new_game(true);
        state_manager::get()->change_state(states::overworld);
        break;
    case button_target::continue_game:
        state_manager::get()->set_new_game(false);
        state_manager::get()->change_state(states::overworld);
        break;
    case button_target::debug:
    {
        break;
    }
    case button_target::options:
        state_manager::get()->push_state(states::options);
        break;
    case button_target::quit:
        state_manager::get()->quit_game();
        break;
    default: break;
    }
}
void button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (button_ != NULL)
    {
        sf::Shader* glowshader = resource_handler::get()->get_shader("faded_outline");
        sf::Shader* bwshader = resource_handler::get()->get_shader("grayscale_luminosity");
        const sf::Shader* defaultshader = states.shader;
        if (!enabled_)
        {
            bwshader->setParameter("texture", *button_->getTexture());
            bwshader->setParameter("factor", 0.0f);
            states.shader = bwshader;
        }
        else if (hover_)
        {
            glowshader->setParameter("texture", *button_->getTexture());
            glowshader->setParameter("img_width", button_->getLocalBounds().width);
            glowshader->setParameter("img_height", button_->getLocalBounds().height);
            glowshader->setParameter("outline_color", utils::rgb(0xFFFF00));
            glowshader->setParameter("outline_width", 5.0);
            states.shader = glowshader;
        }
        target.draw(*button_, states);
        states.shader = defaultshader;
        target.draw(label_, states);
    }
}
void button::set_position(const math::vector2f& new_pos)
{
    if (button_ != NULL)
        button_->setPosition(new_pos);
    label_.setPosition(new_pos + math::vector2f(button_->getLocalBounds().width + 10.0f,
                                                (button_->getLocalBounds().height - label_.getLocalBounds().height) / 2));
}

void button::set_hover(bool status)
{
    hover_ = status;
    label_.setColor(!enabled_ ? utils::rgb(0xBBBBBB) : status ? utils::rgb(0xFFFF00) : utils::rgb(0xffdd4e));
}

bool button::contains(const math::vector2f& point)
{
    if (button_ != NULL)
        return button_->getGlobalBounds().contains(point) || label_.getGlobalBounds().contains(point);
    return false;
}
}   // namespace menu
}   // namespace icarus
