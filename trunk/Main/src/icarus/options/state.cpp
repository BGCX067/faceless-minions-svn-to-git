#include "icarus/options/state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/sound.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/utils.hpp"

namespace icarus
{
namespace options
{
state::state(sf::RenderWindow *const window)
:
    base_state(window)
{
    utils::yth_node* node = resource_handler::get()->get_root_node("ui")->child("options");
    math::vector2f origin(atof(node->child("pos_x")->value().c_str()),
                          atof(node->child("pos_y")->value().c_str()));
    float t_spacing = atof(node->child("text_spacing")->value().c_str());
    float g_spacing = atof(node->child("group_spacing")->value().c_str());
    float h_spacing = atof(node->child("horizontal_spacing")->value().c_str());

    options_title_.setString("Options");
    options_title_.setCharacterSize(30);
    options_title_.setFont(*resource_handler::get()->get_font("text"));
    options_title_.setPosition(origin);

    master_title_.setString("Master Sound");
    master_title_.setCharacterSize(27);
    master_title_.setFont(*resource_handler::get()->get_font("text"));
    master_title_.setPosition(math::vector2f(origin.x, options_title_.getPosition().y +
                                             options_title_.getLocalBounds().height +
                                             g_spacing));
    master_volume_label_.setString("Volume");
    master_volume_label_.setCharacterSize(27);
    master_volume_label_.setFont(*resource_handler::get()->get_font("text"));
    master_volume_label_.setPosition(math::vector2f(origin.x, master_title_.getPosition().y +
                                             master_title_.getLocalBounds().height +
                                             t_spacing));
    master_volume_.setup(node->child("slider"), sound::get()->get_master_volume()*100);
    master_volume_.set_position(math::vector2f(origin.x, master_volume_label_.getPosition().y +
                                                master_volume_label_.getLocalBounds().height +
                                                t_spacing));
    master_enabled_label_.setString("Enabled");
    master_enabled_label_.setCharacterSize(27);
    master_enabled_label_.setFont(*resource_handler::get()->get_font("text"));
    master_enabled_label_.setPosition(math::vector2f(origin.x + h_spacing, master_title_.getPosition().y +
                                             master_title_.getLocalBounds().height +
                                             t_spacing));
    master_enabled_.setup(node->child("checkbox"), sound::get()->get_sound_enabled());
    master_enabled_.set_position(math::vector2f(origin.x + h_spacing, master_volume_label_.getPosition().y +
                                                master_volume_label_.getLocalBounds().height +
                                                t_spacing));

    bgm_title_.setString("Music");
    bgm_title_.setCharacterSize(27);
    bgm_title_.setFont(*resource_handler::get()->get_font("text"));
    bgm_title_.setPosition(math::vector2f(origin.x, master_volume_label_.getPosition().y +
                                             master_volume_label_.getLocalBounds().height +
                                             g_spacing * 2));
    bgm_volume_label_.setString("Volume");
    bgm_volume_label_.setCharacterSize(27);
    bgm_volume_label_.setFont(*resource_handler::get()->get_font("text"));
    bgm_volume_label_.setPosition(math::vector2f(origin.x, bgm_title_.getPosition().y +
                                             bgm_title_.getLocalBounds().height +
                                             t_spacing));
    bgm_volume_.setup(node->child("slider"), sound::get()->get_bgm_volume()*100);
    bgm_volume_.set_position(math::vector2f(origin.x, bgm_volume_label_.getPosition().y +
                                                bgm_volume_label_.getLocalBounds().height +
                                                t_spacing));
    bgm_volume_.set_enabled(sound::get()->get_sound_enabled());
    bgm_enabled_label_.setString("Enabled");
    bgm_enabled_label_.setCharacterSize(27);
    bgm_enabled_label_.setFont(*resource_handler::get()->get_font("text"));
    bgm_enabled_label_.setPosition(math::vector2f(origin.x + h_spacing, bgm_title_.getPosition().y +
                                             bgm_title_.getLocalBounds().height +
                                             t_spacing));
    bgm_enabled_.setup(node->child("checkbox"), sound::get()->get_bgm_enabled());
    bgm_enabled_.set_position(math::vector2f(origin.x + h_spacing, bgm_volume_label_.getPosition().y +
                                                bgm_volume_label_.getLocalBounds().height +
                                                t_spacing));
    bgm_enabled_.set_enabled(sound::get()->get_sound_enabled());

    sfx_title_.setString("Effects");
    sfx_title_.setCharacterSize(27);
    sfx_title_.setFont(*resource_handler::get()->get_font("text"));
    sfx_title_.setPosition(math::vector2f(origin.x, bgm_volume_label_.getPosition().y +
                                             bgm_volume_label_.getLocalBounds().height +
                                             g_spacing * 2));
    sfx_volume_label_.setString("Volume");
    sfx_volume_label_.setCharacterSize(27);
    sfx_volume_label_.setFont(*resource_handler::get()->get_font("text"));
    sfx_volume_label_.setPosition(math::vector2f(origin.x, sfx_title_.getPosition().y +
                                             sfx_title_.getLocalBounds().height +
                                             t_spacing));
    sfx_volume_.setup(node->child("slider"), sound::get()->get_sfx_volume()*100);
    sfx_volume_.set_position(math::vector2f(origin.x, sfx_volume_label_.getPosition().y +
                                                sfx_volume_label_.getLocalBounds().height +
                                                t_spacing));
    sfx_volume_.set_enabled(sound::get()->get_sound_enabled());
    sfx_enabled_label_.setString("Enabled");
    sfx_enabled_label_.setCharacterSize(27);
    sfx_enabled_label_.setFont(*resource_handler::get()->get_font("text"));
    sfx_enabled_label_.setPosition(math::vector2f(origin.x + h_spacing, sfx_title_.getPosition().y +
                                             sfx_title_.getLocalBounds().height +
                                             t_spacing));
    sfx_enabled_.setup(node->child("checkbox"), sound::get()->get_sfx_enabled());
    sfx_enabled_.set_position(math::vector2f(origin.x + h_spacing, sfx_volume_label_.getPosition().y +
                                                sfx_volume_label_.getLocalBounds().height +
                                                t_spacing));
    sfx_enabled_.set_enabled(sound::get()->get_sound_enabled());

    origin.x += atof(node->child("second_col_x")->value().c_str());

    video_options_.setString("Video");
    video_options_.setCharacterSize(27);
    video_options_.setFont(*resource_handler::get()->get_font("text"));
    video_options_.setPosition(math::vector2f(origin.x, options_title_.getPosition().y +
                                             options_title_.getLocalBounds().height +
                                             g_spacing));
    fullscreen_label_.setString("Fullscreen");
    fullscreen_label_.setCharacterSize(27);
    fullscreen_label_.setFont(*resource_handler::get()->get_font("text"));
    fullscreen_label_.setPosition(math::vector2f(origin.x, video_options_.getPosition().y +
                                             video_options_.getLocalBounds().height +
                                             t_spacing));
    fullscreen_.setup(node->child("checkbox"), state_manager::get()->get_fullscreen());
    fullscreen_.set_position(math::vector2f(origin.x, fullscreen_label_.getPosition().y +
                                                fullscreen_label_.getLocalBounds().height +
                                                t_spacing));
    hd_label_.setString("High Definition (1920x1080)");
    hd_label_.setCharacterSize(27);
    hd_label_.setFont(*resource_handler::get()->get_font("text"));
    hd_label_.setPosition(math::vector2f(origin.x, fullscreen_label_.getPosition().y +
                                             fullscreen_label_.getLocalBounds().height +
                                             g_spacing));
    hd_.setup(node->child("checkbox"), state_manager::get()->get_hd_status());
    hd_.set_position(math::vector2f(origin.x, hd_label_.getPosition().y +
                                                hd_label_.getLocalBounds().height +
                                                t_spacing));
    hd_.set_enabled(state_manager::get()->is_hd_supported());

    vsync_label_.setString("Vertical Synchronization (vsync)");
    vsync_label_.setCharacterSize(27);
    vsync_label_.setFont(*resource_handler::get()->get_font("text"));
    vsync_label_.setPosition(math::vector2f(origin.x, hd_label_.getPosition().y +
                                             hd_label_.getLocalBounds().height +
                                             g_spacing));
    vsync_.setup(node->child("checkbox"), state_manager::get()->get_vsync());
    vsync_.set_position(math::vector2f(origin.x, vsync_label_.getPosition().y +
                                                vsync_label_.getLocalBounds().height +
                                                t_spacing));

    return_to_menu_.setString("Return...");
    return_to_menu_.setCharacterSize(30);
    return_to_menu_.setFont(*resource_handler::get()->get_font("text"));
    return_to_menu_.setPosition(20.0f, state_manager::get()->get_dimensions().y -
                                return_to_menu_.getLocalBounds().height - 20.0f);

    quit_game_.setString("Quit game.");
    quit_game_.setCharacterSize(30);
    quit_game_.setFont(*resource_handler::get()->get_font("text"));
    quit_game_.setPosition(state_manager::get()->get_dimensions().x - quit_game_.getLocalBounds().width - 20.0f,
                           state_manager::get()->get_dimensions().y - quit_game_.getLocalBounds().height - 20.0f);
}
state::~state()
{

}

void state::pause_state()
{

}
void state::resume_state()
{

}

void state::handle_input(const std::vector<sf::Event>& key_events)
{
    master_volume_.handle_input(key_events);
    bgm_volume_.handle_input(key_events);
    sfx_volume_.handle_input(key_events);
    for (unsigned i = 0; i < key_events.size(); ++i)
    {
        switch (key_events[i].type)
        {
        case sf::Event::MouseButtonReleased:
            {
                math::vector2f mouse_pos(key_events[i].mouseButton.x, key_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);

                if (return_to_menu_.getGlobalBounds().contains(mouse_pos))
                {
                    save_changes();
                    state_manager::get()->store_data(NULL);
                    state_manager::get()->pop_state();
                    return;
                }
                else if (quit_game_.getGlobalBounds().contains(mouse_pos))
                {
                    save_changes();
                    state_manager::get()->quit_game();
                    return;
                }

                if (master_enabled_.contains(mouse_pos))
                {
                    master_enabled_.toggle();
                    bgm_enabled_.set_enabled(master_enabled_.is_checked());
                    sfx_enabled_.set_enabled(master_enabled_.is_checked());
                    bgm_volume_.set_enabled(master_enabled_.is_checked());
                    sfx_volume_.set_enabled(master_enabled_.is_checked());
                }
                else if (bgm_enabled_.contains(mouse_pos))
                {
                    bgm_enabled_.toggle();
                }
                else if (sfx_enabled_.contains(mouse_pos))
                {
                    sfx_enabled_.toggle();
                }
                else if (fullscreen_.contains(mouse_pos))
                {
                    fullscreen_.toggle();
                }
                else if (hd_.contains(mouse_pos))
                {
                    hd_.toggle();
                }
                else if (vsync_.contains(mouse_pos))
                {
                    vsync_.toggle();
                }
            }
            break;
        case sf::Event::MouseMoved:
            {
                math::vector2f mouse_pos(key_events[i].mouseMove.x, key_events[i].mouseMove.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);

                if (return_to_menu_.getGlobalBounds().contains(mouse_pos))
                {
                    return_to_menu_.setColor(utils::rgb(0xFFFF00));
                }
                else
                {
                    return_to_menu_.setColor(utils::rgb(0xFFFFFF));
                }
                if (quit_game_.getGlobalBounds().contains(mouse_pos))
                {
                    quit_game_.setColor(utils::rgb(0xFFFF00));
                }
                else
                {
                    quit_game_.setColor(utils::rgb(0xFFFFFF));
                }
            }
            break;
        default: break;
        }
    }
}

void state::save_changes()
{
    sound::get()->set_bgm_volume(float(bgm_volume_.get_value())/100.0f);
    sound::get()->set_sfx_volume(float(sfx_volume_.get_value())/100.0f);
    sound::get()->set_master_volume(float(master_volume_.get_value())/100.0f);
    sound::get()->set_sound_enabled(master_enabled_.is_checked());
    sound::get()->set_bgm_enabled(bgm_enabled_.is_checked());
    sound::get()->set_sfx_enabled(sfx_enabled_.is_checked());

    state_manager::get()->set_window_options(fullscreen_.is_checked(),
                                             hd_.is_checked(),
                                             vsync_.is_checked());
}

void state::handle_logic()
{

}
void state::draw() const
{
    window_->draw(options_title_);
    window_->draw(return_to_menu_);
    window_->draw(quit_game_);

    window_->draw(master_title_);
    window_->draw(master_volume_label_);
    window_->draw(master_volume_);
    window_->draw(master_enabled_label_);
    window_->draw(master_enabled_);

    window_->draw(bgm_title_);
    window_->draw(bgm_volume_label_);
    window_->draw(bgm_volume_);
    window_->draw(bgm_enabled_label_);
    window_->draw(bgm_enabled_);

    window_->draw(sfx_title_);
    window_->draw(sfx_volume_label_);
    window_->draw(sfx_volume_);
    window_->draw(sfx_enabled_label_);
    window_->draw(sfx_enabled_);

    window_->draw(video_options_);
    window_->draw(fullscreen_label_);
    window_->draw(hd_label_);
    window_->draw(vsync_label_);
    window_->draw(fullscreen_);
    window_->draw(hd_);
    window_->draw(vsync_);
}
}   // namespace options
}   // namespace icarus

