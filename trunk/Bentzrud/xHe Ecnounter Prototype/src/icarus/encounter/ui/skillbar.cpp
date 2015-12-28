#include "icarus/encounter/ui/skillbar.hpp"
#include "icarus/encounter/character_hero.hpp"
#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
skillbar::skillbar(state* state)
:
    state_(state),
    hero_(NULL)
{
    utils::yth_node* ui_root = resource_handler::get()->get_root_node("ui");
    std::string bg_path = "images/interface/"+ui_root->child("skill_bar")->child("background")->value();
    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(bg_path))
        background_ = sf::Sprite(*bg_tex);

    float root_x = atof(ui_root->child("skill_bar")->child("position_x")->value().c_str());
    float root_y = atof(ui_root->child("skill_bar")->child("position_y")->value().c_str());
    background_.setPosition(root_x, root_y);

    for (unsigned i = 0; i < 5; ++i)
    {
        if (sf::Texture* bt_tex = resource_handler::get()->get_texture("images/interface/icons/"+ui_root->child("skill_bar")->child("default_icon")->value()))
            buttons_[i] = sf::Sprite(*bt_tex);
        else
            continue;
        float pos_x = atof(ui_root->child("skill_bar")->child("skill", i)->child("pos_x")->value().c_str());
        float pos_y = atof(ui_root->child("skill_bar")->child("skill", i)->child("pos_y")->value().c_str());
        buttons_[i].setPosition(root_x + pos_x, root_y + pos_y);
    }
}
skillbar::~skillbar()
{

}

void skillbar::handle_input(const sf::Event& input_event)
{
    switch (input_event.type)
    {
    case sf::Event::KeyPressed:
        switch(input_event.key.code)
        {
        case sf::Keyboard::Q:
            state_->cast_player_skill(hero_, 1); break;
        case sf::Keyboard::W:
            state_->cast_player_skill(hero_, 2); break;
        case sf::Keyboard::E:
            state_->cast_player_skill(hero_, 3); break;
        case sf::Keyboard::R:
            state_->cast_player_skill(hero_, 4); break;
        default: break;
        }
        break;
    case sf::Event::MouseButtonReleased:
    {
        math::vector2i mouse_pos(input_handler::get()->get_mouse_xy());
        for (unsigned i = 0; i < 5; ++i)
        {
            if (buttons_[i].getGlobalBounds().contains(sf::Vector2f(mouse_pos.get_sf_vector())))
            {
                if (input_event.mouseButton.button == sf::Mouse::Left)
                {
                    state_->cast_player_skill(hero_, i);
                }
                else if (input_event.mouseButton.button == sf::Mouse::Right &&
                         hero_->get_skill(i).get_flags() & skill_flags::CAN_AUTOCAST)
                {
                    hero_->get_skill(i).set_autocast(!hero_->get_skill(i).get_autocast());
                }
            }
        }
        break;
    }
    default: break;
    }
}
void skillbar::draw(sf::RenderTarget& target,
                       sf::RenderStates states) const
{
    target.draw(background_, states);
    for (unsigned button = 0; button < 5; ++button)
    {
        target.draw(buttons_[button], states);
    }
}
void skillbar::set_selected_hero(character_hero* hero)
{
    hero_ = hero;
    utils::yth_node* icon_root = resource_handler::get()->get_root_node("ui")->child("icons");
    std::string icon = "";
    for (int i = 0; i < 5; ++i)
    {
        icon = "images/interface/icons/"+icon_root->child(hero_->get_skill(i).get_name())->value();
        buttons_[i].setTexture(*resource_handler::get()->get_texture(icon));
    }
}
} // namespace ui
} // namespace encounter
} // namespace icarus
