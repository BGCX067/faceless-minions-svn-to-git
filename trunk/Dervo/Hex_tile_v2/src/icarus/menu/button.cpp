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
    hover_(false)
{
    utils::yth_node* ui_root = resource_handler::get()->get_root_node("ui");
    std::string button_path = ui_root->child("menu_buttons")->child(button_type)->child("sprite")->value();
    if (sf::Texture* button_tex = resource_handler::get()->get_texture(button_path))
        button_ = new sf::Sprite(*button_tex);
    label_.setString(ui_root->child("menu_buttons")->child(button_type)->child("label")->value());
    label_.setCharacterSize(30);
    label_.setFont(*resource_handler::get()->get_font("text"));
    label_.setColor(utils::rgb(0xFFFFFF));
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
        state_manager::get()->change_state(states::overworld);
        break;
    case button_target::continue_game:
        break;
    case button_target::debug:
    {
        // TODO:: load test encounter
        //START DEBUG DATA -----
        icarus::encounter::data* test_encounter = new icarus::encounter::data();

        icarus::encounter::hero_data p1data;
        p1data.name_ = "TestChar 1";
        p1data.skill_levels_[0] = 3;
        p1data.skill_levels_[1] = 3;
        p1data.skill_levels_[2] = 3;
        p1data.skill_levels_[3] = 1;
        p1data.health_ = 100;
        p1data.current_health_ = 1.0f;
        p1data.strength_ = 20;
        p1data.agility_ = 20;
        p1data.intelligence_ = 20;
        p1data.move_speed_ = 300.0f;
        p1data.class_ = icarus::encounter::hero_class::mage;
        test_encounter->player_party_.push_back(p1data);

        icarus::encounter::hero_data p2data;
        p2data.name_ = "TestChar 2";
        p2data.skill_levels_[0] = 3;
        p2data.skill_levels_[1] = 3;
        p2data.skill_levels_[2] = 3;
        p2data.skill_levels_[3] = 1;
        p2data.health_ = 100;
        p2data.current_health_ = 1.0f;
        p2data.strength_ = 100;
        p2data.agility_ = 100;
        p2data.intelligence_ = 100;
        p2data.move_speed_ = 300.0f;
        p2data.class_ = icarus::encounter::hero_class::mage;
        test_encounter->player_party_.push_back(p2data);

        test_encounter->encounter_node_ = icarus::resource_handler::get()->get_root_node("encounters")->child("region", icarus::resource_handler::get()->get_root_node("encounters")->child_count("region") - 1)->child("test_encounter")->child("combat");
        test_encounter->current_scenario_ = 0;
        test_encounter->scenario_count_ = test_encounter->encounter_node_->child_count("scenario");

        icarus::state_manager::get()->store_data(test_encounter);
        //END DEBUG DATA -----
        state_manager::get()->change_state(states::encounter);
        break;
    }
    case button_target::options:
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
        const sf::Shader* defaultshader = states.shader;
        if (hover_)
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
    label_.setColor(status ? utils::rgb(0xFFFF00) : utils::rgb(0xFFFFFF));
}

bool button::contains(const math::vector2f& point)
{
    if (button_ != NULL)
        return button_->getGlobalBounds().contains(point) || label_.getGlobalBounds().contains(point);
    return false;
}
}   // namespace menu
}   // namespace icarus
