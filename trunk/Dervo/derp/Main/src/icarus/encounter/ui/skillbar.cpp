#include "icarus/encounter/ui/skillbar.hpp"
#include "icarus/encounter/character_hero.hpp"
#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/math/vector2.hpp"

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

    float root_x = (state_manager::get()->get_dimensions().x - background_.getLocalBounds().width) / 2;
    float root_y = (state_manager::get()->get_dimensions().y - background_.getLocalBounds().height);
    background_.setPosition(root_x, root_y);

    for (unsigned i = 0; i < 5; ++i)
    {
        if (sf::Texture* bt_tex = resource_handler::get()->get_texture("images/interface/icons/"+ui_root->child("skill_bar")->child("default_icon")->value()))
        {
            buttons_[i] = new icon(bt_tex);
        }
        else
        {
            buttons_[i] = NULL;
            continue;
        }
        float pos_x = atof(ui_root->child("skill_bar")->child("skill", i)->child("pos_x")->value().c_str());
        float pos_y = atof(ui_root->child("skill_bar")->child("skill", i)->child("pos_y")->value().c_str());
        buttons_[i]->set_position(root_x + pos_x, root_y + pos_y);
    }

    portrait_.setTexture(*resource_handler::get()->get_texture("images/interface/icons/icon_empty.png"));
    float pos_x = atof(ui_root->child("skill_bar")->child("portrait")->child("pos_x")->value().c_str());
    float pos_y = atof(ui_root->child("skill_bar")->child("portrait")->child("pos_y")->value().c_str());
    portrait_.setPosition(root_x + pos_x, root_y + pos_y);
}
skillbar::~skillbar()
{
    for (unsigned i = 0; i < 5; ++i)
    {
        if (buttons_[i] != NULL)
            delete buttons_[i];
    }
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
        math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos<float>(math::vector2f(input_event.mouseButton.x, input_event.mouseButton.y));
        int p1 = -1;
        int p2 = -1;
        for (unsigned i = 0; i < 5; ++i)
        {
            if (buttons_[i]->get_bounds().contains(sf::Vector2f(mouse_pos.get_sf_vector())) &&
                !buttons_[i]->is_disabled())
            {
                if (p1 == -1)
                    p1 = i;
                else if (p2 == -1)
                    p2 = i;
            }
        }

        if (p1 != -1 && p2 != -1)
        {
            sf::FloatRect intersection;
            if (buttons_[p1]->get_bounds().intersects(buttons_[p2]->
                                                get_bounds(),
                                                intersection))
            {
                math::vector2f v1(intersection.left, intersection.top);
                math::vector2f v2(intersection.left,
                                intersection.top + intersection.height);
                math::vector2f v3(intersection.left + intersection.width,
                                intersection.top +
                                (buttons_[p1]->get_position().y >
                                buttons_[p2]->get_position().y ?
                                intersection.height : 0));

                if (math::point_in_triangle(v1, v2, v3, mouse_pos))
                {
                    if (input_event.mouseButton.button == sf::Mouse::Left)
                    {
                        state_->cast_player_skill(hero_, p1);
                    }
                    else if (input_event.mouseButton.button == sf::Mouse::Right &&
                             hero_->get_skill(p1).get_flags() & skill_flags::CAN_AUTOCAST)
                    {
                        hero_->get_skill(p1).set_autocast(!hero_->get_skill(p1).get_autocast());
                        buttons_[p1]->set_autocast_enabled(hero_->get_skill(p1).get_autocast());
                    }
                }
                else
                {
                    if (input_event.mouseButton.button == sf::Mouse::Left)
                    {
                        state_->cast_player_skill(hero_, p2);
                    }
                    else if (input_event.mouseButton.button == sf::Mouse::Right &&
                             hero_->get_skill(p2).get_flags() & skill_flags::CAN_AUTOCAST)
                    {
                        hero_->get_skill(p2).set_autocast(!hero_->get_skill(p2).get_autocast());
                        buttons_[p2]->set_autocast_enabled(hero_->get_skill(p2).get_autocast());
                    }
                }
            }
        }
        else if (p1 != -1)
        {
            if (input_event.mouseButton.button == sf::Mouse::Left)
            {
                state_->cast_player_skill(hero_, p1);
            }
            else if (input_event.mouseButton.button == sf::Mouse::Right &&
                    hero_->get_skill(p1).get_flags() & skill_flags::CAN_AUTOCAST)
            {
                hero_->get_skill(p1).set_autocast(!hero_->get_skill(p1).get_autocast());
                buttons_[p1]->set_autocast_enabled(hero_->get_skill(p1).get_autocast());
            }
        }
        break;
    }
    case sf::Event::MouseMoved:
    {
        math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos<float>(math::vector2f(input_event.mouseMove.x, input_event.mouseMove.y));
        int p1 = -1;
        int p2 = -1;
        for (unsigned i = 0; i < 5; ++i)
        {
            // TODO:: barycentric coords
            if (buttons_[i]->get_bounds().contains(mouse_pos) &&
                !buttons_[i]->is_disabled())
            {
                if (p1 == -1)
                    p1 = i;
                else if (p2 == -1)
                    p2 = i;
            }
            else
            {
                // check if hover is shown, if so, hide hover
                if (buttons_[i]->get_outline_visible())
                    buttons_[i]->set_outline_visible(false);
            }
        }

        if (p1 != -1 && p2 != -1)
        {
            sf::FloatRect intersection;
            if (buttons_[p1]->get_bounds().intersects(buttons_[p2]->
                                                get_bounds(),
                                                intersection))
            {
                math::vector2f v1(intersection.left, intersection.top);
                math::vector2f v2(intersection.left,
                                intersection.top + intersection.height);
                math::vector2f v3(intersection.left + intersection.width,
                                intersection.top +
                                (buttons_[p1]->get_position().y >
                                buttons_[p2]->get_position().y ?
                                intersection.height : 0));

                if (math::point_in_triangle(v1, v2, v3, mouse_pos))
                {
                    if (!buttons_[p1]->get_outline_visible())
                        buttons_[p1]->set_outline_visible(true);
                    if (buttons_[p2]->get_outline_visible())
                        buttons_[p2]->set_outline_visible(false);
                }
                else
                {
                    if (!buttons_[p2]->get_outline_visible())
                        buttons_[p2]->set_outline_visible(true);
                    if (buttons_[p1]->get_outline_visible())
                        buttons_[p1]->set_outline_visible(false);
                }
            }
        }
        else if (p1 != -1)
        {
            if (!buttons_[p1]->get_outline_visible())
                buttons_[p1]->set_outline_visible(true);
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
    target.draw(portrait_, states);
    for (unsigned button = 0; button < 5; ++button)
    {
        target.draw(*buttons_[button], states);
    }
}
void skillbar::set_selected_hero(character_hero* hero)
{
    hero_ = hero;

    utils::yth_node* icon_root = resource_handler::get()->get_root_node("ui")->child("icons");
    std::string icon = "";
    std::string portrait = "";
    portrait_.setTexture(*hero->get_portrait_ptr());
    for (int i = 0; i < 5; ++i)
    {
        if (hero->get_skill(i).is_leveled())
        {
            buttons_[i]->set_disabled((hero->get_skill(i).get_level() == 0));
        }
        icon = "images/interface/icons/"+icon_root->child(hero_->get_skill(i).get_name())->value();
        buttons_[i]->set_texture(resource_handler::get()->get_texture(icon));
        buttons_[i]->set_tt_data(resource_handler::get()->get_root_node("skills")->child(hero->get_skill(i).get_name())->child("tooltip"));

        if (hero->get_skill(i).is_ready())
        {
            buttons_[i]->stop_cooldown();
        }
        else
        {
            buttons_[i]->set_cooldown(hero->get_skill(i).get_cooldown(),
                                      hero->get_skill(i).current_cooldown());
        }
        buttons_[i]->set_autocast_enabled(hero->get_skill(i).get_autocast());
    }
}
void skillbar::update()
{
    for (unsigned i = 0; i < 5; ++i)
        buttons_[i]->update();
}

void skillbar::set_cooldown(unsigned index, float duration)
{
    buttons_[index]->set_cooldown(duration);
}
} // namespace ui
} // namespace encounter
} // namespace icarus
