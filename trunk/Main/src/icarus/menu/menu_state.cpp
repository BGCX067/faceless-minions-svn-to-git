#include "icarus/menu/menu_state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/math/vector2.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/sound.hpp"
#include "icarus/state_manager.hpp"

namespace icarus
{
namespace menu
{
menu_state::menu_state(sf::RenderWindow *const window)
:
    base_state(window),
    background_(NULL),
    fader_(utils::rgb(0), 1.0f, true),
    clicked_index_(-1)
{
    utils::yth_node* menu_node = resource_handler::get()->get_root_node("ui")->child("main_menu");
    std::string bg_path = menu_node->child("background")->value();
    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(bg_path))
        background_ = new sf::Sprite(*bg_tex);

    for (unsigned i = 0; i < menu_node->child_count("button"); ++i)
    {
        buttons_.push_back(new button(menu_node->child("button", i)->value()));
        if (static_cast<button_target::value>(atoi(menu_node->child("button", i)->child("target")->value().c_str())) == button_target::continue_game)
            buttons_.back()->set_enabled((menu_node->child("button", i)->child("enabled")->value() == "true") && utils::file_exists("savegames/autosave.dag"));
        else
            buttons_.back()->set_enabled((menu_node->child("button", i)->child("enabled")->value() == "true"));
        buttons_.back()->set_target(static_cast<button_target::value>(atoi(menu_node->child("button", i)->child("target")->value().c_str())));
        buttons_.back()->set_position(utils::to_hd(math::vector2f(atof(menu_node->child("button", i)->child("pos_x")->value().c_str()),
                                                     atof(menu_node->child("button", i)->child("pos_y")->value().c_str()))));
    }

    utils::yth_node* audio_node = resource_handler::get()->get_root_node("audio");
    sound::get()->load_bgm(audio_node->child(menu_node->child("music")->value())->value(), true, true);

    sound::get()->load_sfx(audio_node->child("sfx")->child("menu_hover")->value(), "hover");
    sound::get()->load_sfx(audio_node->child("sfx")->child("menu_select")->value(), "select");
    fader_.reset(false);
}
menu_state::~menu_state()
{
    sound::get()->stop_bgm();
    sound::get()->unload_all();
    if (background_ != NULL)
        delete background_;
    for (unsigned i = 0; i < buttons_.size(); ++i)
        delete buttons_[i];
}
void menu_state::pause_state()
{
    sound::get()->stop_bgm();
    sound::get()->unload_all();
}
void menu_state::resume_state()
{
    utils::yth_node* menu_node = resource_handler::get()->get_root_node("ui")->child("main_menu");
    utils::yth_node* audio_node = resource_handler::get()->get_root_node("audio");

    std::string bg_path = menu_node->child("background")->value();
    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(bg_path))
        background_ = new sf::Sprite(*bg_tex);

    for (unsigned i = 0; i < buttons_.size(); ++i)
    {
        buttons_[i]->set_position(utils::to_hd(math::vector2f(atof(menu_node->child("button", i)->child("pos_x")->value().c_str()),
                                                     atof(menu_node->child("button", i)->child("pos_y")->value().c_str()))));
    }

    sound::get()->load_bgm(audio_node->child(menu_node->child("music")->value())->value(), true, true);
    sound::get()->load_sfx(audio_node->child("sfx")->child("menu_hover")->value(), "hover");
    sound::get()->load_sfx(audio_node->child("sfx")->child("menu_select")->value(), "select");
    fader_.reset(false);
    clicked_index_ = -1;
}
void menu_state::handle_input(const std::vector<sf::Event>& key_events)
{
    double delta = state_manager::get()->get_delta();
    if (fader_.is_active())
    {
        if (fader_.update(delta) && clicked_index_ != -1)
        {
            buttons_[clicked_index_]->click(window_);
            //clicked_index_ = -1;
        }
        return;
    }
    for (unsigned i = 0; i < key_events.size(); ++i)
    {
        if (key_events[i].type == sf::Event::MouseButtonReleased &&
            key_events[i].mouseButton.button == sf::Mouse::Left)
        {
            math::vector2f mouse_pos(input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseButton.x,
                                                                                            key_events[i].mouseButton.y)));
            for (unsigned k = 0; k < buttons_.size(); ++k)
            {
                if (buttons_[k]->contains(mouse_pos) &&
                    buttons_[k]->is_enabled())
                {
                    clicked_index_ = k;
                    fader_.reset(true);
                    sound::get()->play_sfx("select");
                }
            }
        }
        else if (key_events[i].type == sf::Event::MouseMoved)
        {
            math::vector2f mouse_pos(input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseMove.x,
                                                                                            key_events[i].mouseMove.y)));
            for (unsigned k = 0; k < buttons_.size(); ++k)
            {
                if (buttons_[k]->contains(mouse_pos) && !buttons_[k]->get_hover() && buttons_[k]->is_enabled())
                    sound::get()->play_sfx("hover");
                buttons_[k]->set_hover(buttons_[k]->contains(mouse_pos));
            }
        }
    }
}
void menu_state::handle_logic()
{

}
void menu_state::draw() const
{
    if (background_)
        window_->draw(*background_);
    for (unsigned i = 0; i < buttons_.size(); ++i)
        window_->draw(*buttons_[i]);
    if (fader_.is_active() || clicked_index_ != -1)
        window_->draw(fader_);
}
}   // namespace menu
}   // namespace icarus
