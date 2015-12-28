#include <ctime>
#include <fstream>
#include <windows.h>

#include "icarus/state_manager.hpp"
#include "icarus/utils.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/encounter/state.hpp"
#include "icarus/overworld/overworld_state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/ui/tooltip_handler.hpp"
#include "icarus/menu/menu_state.hpp"
#include "icarus/endgame/state.hpp"
#include "icarus/sound.hpp"
#include "icarus/config.hpp"
#include "icarus/options/state.hpp"

namespace icarus
{
state_manager* state_manager::instance_ = NULL;

state_manager::state_manager() :    BIG_ENDIAN_(utils::is_big_endian()),
                                    new_game_(true),
                                    delta_(0),
                                    prev_delta_(0),
                                    stored_data_(NULL),
                                    window_(NULL),
                                    debug_flags_(0x0),
                                    cursor_(NULL),
                                    full_hd_(false),
                                    vsync_enabled_(false),
                                    is_fullscreen_(false),
                                    hd_is_supported_(false)
{
}

state_manager::~state_manager()
{
}

void state_manager::run_game()
{
    while (window_->isOpen())
    {
        for (unsigned i = 0; i < state_events_.size(); ++i)
        {
            switch(state_events_[i].action_)
            {
            case state_actions::push:
                push(state_events_[i].state_);
                break;
            case state_actions::pop:
                pop();
                break;
            case state_actions::change:
                change(state_events_[i].state_);
                break;
            case state_actions::quit:
                quit();
                break;
            }
        }
        state_events_.clear();
        if (state_stack_.size() == 0)
        {
            save_config();
            window_->close();
            return;
        }
        sound::get()->update();
        double current_time = 0.0;
        sf::Event event;
        sf::Event* last_mouse_move = NULL;
        std::vector<sf::Event> key_events;
        while (window_->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window_->close();
            if (event.type == sf::Event::KeyPressed ||
                event.type == sf::Event::KeyReleased ||
                event.type == sf::Event::TextEntered ||
                event.type == sf::Event::MouseButtonPressed ||
                event.type == sf::Event::MouseButtonReleased)
            {
                key_events.push_back(event);
            }
            // screenshot hook
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::F9)
                delta_ -= take_screenshot();
            else if (event.type == sf::Event::KeyPressed)
                update_debug_flags(event);
            else if (event.type == sf::Event::MouseMoved)
            {
                last_mouse_move = &event;
            }
        }
        if (last_mouse_move != NULL)
        {
            key_events.push_back(*last_mouse_move);
            cursor_->set_world_pos(input_handler::get()->convert_mouse_pos(math::vector2f(last_mouse_move->mouseMove.x,
                                                                                          last_mouse_move->mouseMove.y)));
            ui::tooltip_handler::get()->update_tooltip(*last_mouse_move);
        }
        debug_data_.events_ += 0.0;//(current_time = game_time_.getElapsedTime().asMicroseconds() - prev_delta_);

        state_stack_.back()->handle_input(key_events);
        debug_data_.input_ += 0.0;//(current_time = game_time_.getElapsedTime().asMicroseconds() - prev_delta_ - current_time);

        current_time = game_time_.getElapsedTime().asMicroseconds();
        state_stack_.back()->handle_logic();
        debug_data_.logic_ += (current_time = game_time_.getElapsedTime().asMicroseconds() - current_time);

        window_->clear();
        state_stack_.back()->draw();

        // tooltips! possibly move this.
        window_->draw(*ui::tooltip_handler::get());

        window_->draw(*cursor_);
        draw_debug();
        window_->display();
        debug_data_.draw_ += 0.0;//(current_time = game_time_.getElapsedTime().asMicroseconds() - prev_delta_ - current_time);
        delta_ = game_time_.getElapsedTime().asMicroseconds() - prev_delta_;
        prev_delta_ = game_time_.getElapsedTime().asMicroseconds();

        debug_data_.frame_counter_++;
        debug_data_.debug_timer_ += get_delta();
    }
}

int state_manager::take_screenshot()
{
    int delta = game_time_.getElapsedTime().asMicroseconds();
    time_t curr_time;
    time(&curr_time);
    struct tm * timeinfo;
    timeinfo = localtime(&curr_time);
    std::string timestring =    utils::to_str(timeinfo->tm_year + 1900) + "-" +
                                utils::to_str(timeinfo->tm_mon + 1, 2) + "-" +
                                utils::to_str(timeinfo->tm_mday, 2);
    std::string filename = "";
    int counter = 0;
    do
    {
        filename = std::string("screenshots/vanadia-") +
                    timestring + "-" +
                    utils::to_str(counter, 3) +
                    ".png";
        counter++;
    } while (utils::file_exists(filename));
    window_->capture().saveToFile(filename);
    return game_time_.getElapsedTime().asMicroseconds() - delta;
}

void state_manager::update_debug_flags(const sf::Event& event)
{
    if (event.key.code == sf::Keyboard::F && event.key.control)
        debug_flags_ = (debug_flags_ & debug::SHOW_FPS ?
                        debug_flags_ & ~debug::SHOW_FPS :
                        debug_flags_ | debug::SHOW_FPS);
    else if (event.key.code == sf::Keyboard::X && event.key.control)
        debug_flags_ = (debug_flags_ & debug::SHOW_TIMINGS ?
                        debug_flags_ & ~debug::SHOW_TIMINGS :
                        debug_flags_ | debug::SHOW_TIMINGS);
    else if (event.key.code == sf::Keyboard::P && event.key.control)
        debug_flags_ = (debug_flags_ & debug::SHOW_PROFILER ?
                        debug_flags_ & ~debug::SHOW_PROFILER :
                        debug_flags_ | debug::SHOW_PROFILER);
}

void state_manager::draw_debug()
{
    if (debug_flags_ & debug::SHOW_FPS)
    {
        sf::Text fps;
        fps.setFont(*resource_handler::get()->get_font("console"));
        fps.setString(std::string("fps: " + utils::to_str<int>(1000000/delta_)));
        fps.setCharacterSize(10);
        fps.setPosition(window_->mapPixelToCoords(sf::Vector2i(0.0f, window_->getSize().y - fps.getLocalBounds().height)));
        window_->draw(fps);
    }
    if (debug_flags_ & debug::SHOW_TIMINGS)
    {
        window_->draw(debug_data_.t_events_);
        window_->draw(debug_data_.t_input_);
        window_->draw(debug_data_.t_logic_);
        window_->draw(debug_data_.t_draw_);
    }
    if (debug_flags_ & debug::SHOW_PROFILER)
    {
        window_->draw(debug_data_.t_profiler_);
    }
    if (debug_data_.debug_timer_ >= 1.0)
    {
        debug_data_.t_events_.setString(std::string("events: " + utils::to_str<double>(debug_data_.events_ / debug_data_.frame_counter_) + "ms"));
        debug_data_.t_input_.setString(std::string("input: " + utils::to_str<double>(debug_data_.input_ / debug_data_.frame_counter_) + "ms"));
        debug_data_.t_logic_.setString(std::string("logic: " + utils::to_str<double>(debug_data_.logic_ / debug_data_.frame_counter_) + "ms"));
        debug_data_.t_draw_.setString(std::string("draw: " + utils::to_str<double>(debug_data_.draw_ / debug_data_.frame_counter_) + "ms"));
        debug_data_.reset();
    }
}

void state_manager::initialize(const std::string& config_path)
{
    config_ = new config();
    config_->load_config_file(config_path);

    uint8_t fullscreen = (config_->get_value("fullscreen", false) ? sf::Style::Fullscreen : 0);
    full_hd_ = config_->get_value("full_hd", false);
    sf::VideoMode hd_video(1920, 1080);
    sf::VideoMode sd_video(1280, 720);
    if (!hd_video.isValid())
    {
        hd_is_supported_ = false;
    }
    else
    {
        hd_is_supported_ = true;
    }
    if (!sd_video.isValid() && !hd_is_supported_)
    {
        print_err("Fatal error!", "No valid video modes found, I'm terribly sorry.\nPlease use a screen that supports either 1920x1080 or 1280x720 resolutions!");
        return;
    }

    window_ = new sf::RenderWindow((full_hd_ && hd_is_supported_ ? hd_video : sd_video),
                                   config_->get_value("window_title", "Encounter Prototype"),
                                   fullscreen);
    if (config_->get_value("frame_limit_enabled", false))
        window_->setFramerateLimit(config_->get_value("frame_limit", 60));
    window_->setVerticalSyncEnabled(vsync_enabled_);
    is_fullscreen_ = bool(fullscreen);

    icarus::input_handler::get()->setup_input(window_);
    window_->setKeyRepeatEnabled(false);

    sound::get()->set_sound_enabled(config_->get_value("master_enabled", true));
    sound::get()->set_master_volume(config_->get_value("master_volume", 1.0f, true));
    sound::get()->set_bgm_enabled(config_->get_value("bgm_enabled", true));
    sound::get()->set_bgm_volume(config_->get_value("bgm_volume", 1.0f, true));
    sound::get()->set_sfx_enabled(config_->get_value("sfx_enabled", true));
    sound::get()->set_sfx_volume(config_->get_value("sfx_volume", 1.0f, true));

    int initial_state = config_->get_value("initial_state",
                                          static_cast<int>(states::menu));

    yth_handler_.load(config_->get_value("resource_index", "resources/index.yth"));
    utils::yth_node* resource_node = yth_handler_.node(0);
    for (unsigned i = 0; i < resource_node->child_count(); ++i)
    {
        yth_handler_.load(resource_node->child(i)->value());
        resource_handler::get()->add_root_node(yth_handler_.node(0),
                                                       resource_node->child(i)->key());
    }
    delete resource_node;

    // load fonts
    for (unsigned i = 0; i < resource_handler::get()->get_root_node("ui")->child("fonts")->child_count(); ++i)
    {
        resource_handler::get()->load_font(resource_handler::get()->get_root_node("ui")->child("fonts")->child(i)->key(),
                                           resource_handler::get()->get_root_node("ui")->child("fonts")->child(i)->value());
    }
    // load shaders
    for (unsigned i = 0; i < resource_handler::get()->get_root_node("ui")->child("shaders")->child_count(); ++i)
    {
        resource_handler::get()->load_shader(resource_handler::get()->get_root_node("ui")->child("shaders")->child(i)->key(),
                                             resource_handler::get()->get_root_node("ui")->child("shaders")->child(i)->value());
    }

    debug_data_.t_events_.setFont(*resource_handler::get()->get_font("console"));
    debug_data_.t_events_.setPosition(window_->mapPixelToCoords(sf::Vector2i(0.0f, window_->getSize().y - 50)));
    debug_data_.t_input_.setFont(*resource_handler::get()->get_font("console"));
    debug_data_.t_input_.setPosition(window_->mapPixelToCoords(sf::Vector2i(0.0f, window_->getSize().y - 40)));
    debug_data_.t_logic_.setFont(*resource_handler::get()->get_font("console"));
    debug_data_.t_logic_.setPosition(window_->mapPixelToCoords(sf::Vector2i(0.0f, window_->getSize().y - 30)));
    debug_data_.t_draw_.setFont(*resource_handler::get()->get_font("console"));
    debug_data_.t_draw_.setPosition(window_->mapPixelToCoords(sf::Vector2i(0.0f, window_->getSize().y - 20)));
    debug_data_.t_profiler_.setFont(*resource_handler::get()->get_font("console"));
    debug_data_.t_profiler_.setPosition(window_->mapPixelToCoords(sf::Vector2i(0.0f, window_->getSize().y - 60)));

    // setup cursor
    window_->setMouseCursorVisible(false);
    cursor_ = new encounter::animate();
    utils::yth_node* cursor_sheet = resource_handler::get()->
                                        get_root_node("sprite_sheets")->
                                        child(resource_handler::get()->
                                              get_root_node("ui")->
                                              child("cursor")->
                                              child("sprite_sheet")->value());
    cursor_->load_sprite(cursor_sheet->child("cursor"),
                         cursor_sheet->child("path")->value());
    cursor_->set_animation_state("default");
    cursor_->update_anim();

    initial_state_ = static_cast<states::state>(initial_state);
}

void state_manager::save_config()
{
    std::ofstream file_stream;
    file_stream.open("settings.cfg", std::ios::out);

    // video settings
    file_stream << "[Video]" << std::endl;
    file_stream << "full_hd=" << (full_hd_ ? "1" : "0") << std::endl;
    file_stream << "frame_limit_enabled=" << "0" << std::endl;
    file_stream << "frame_limit=" << "60" << std::endl;
    file_stream << "fullscreen=" << (is_fullscreen_ ? "1" : "0") << std::endl;
    file_stream << "vsync=" << (vsync_enabled_ ? "1" : "0") << std::endl;

    // audio
    file_stream << "[Audio]" << std::endl;
    file_stream << "master_enabled=" << (sound::get()->get_sound_enabled() ? "1" : "0") << std::endl;
    file_stream << "master_volume=" << utils::to_str(sound::get()->get_master_volume()) << std::endl;
    file_stream << "bgm_enabled=" << (sound::get()->get_bgm_enabled() ? "1" : "0") << std::endl;
    file_stream << "bgm_volume=" << utils::to_str(sound::get()->get_bgm_volume()) << std::endl;
    file_stream << "sfx_enabled=" << (sound::get()->get_sfx_enabled() ? "1" : "0") << std::endl;
    file_stream << "sfx_volume=" << utils::to_str(sound::get()->get_sfx_volume()) << std::endl;

    // resources
    file_stream << "[Resources]" << std::endl;
    file_stream << "resource_index=" << "resources/index.yth" << std::endl;

    // misc
    file_stream << "[Miscellaneous]" << std::endl;
    file_stream << "window_title=" << "\"Vanadia\" Prototype - Icarus Rising Studios" << std::endl;

    file_stream.close();
}

void state_manager::start()
{
    switch (initial_state_)
    {
    case states::intro:
    case states::menu:
        state_stack_.push_back(new icarus::menu::menu_state(window_));
        break;
    case states::overworld:
        state_stack_.push_back(new icarus::overworld::overworld_state(window_));
        break;
    case states::encounter:
        state_stack_.push_back(new icarus::encounter::state(window_));
        break;
    case states::endgame:
        state_stack_.push_back(new icarus::endgame::state(window_));
        break;
    case states::options:
        state_stack_.push_back(new icarus::options::state(window_));
        break;
    }
    game_time_.restart();
    run_game();
}

void state_manager::change(states::state new_state)
{
    delete state_stack_.back();
    state_stack_.pop_back();
    switch (new_state)
    {
    case states::intro:
    case states::menu:
        state_stack_.push_back(new icarus::menu::menu_state(window_));
        break;
    case states::overworld:
        state_stack_.push_back(new icarus::overworld::overworld_state(window_));
        break;
    case states::encounter:
        state_stack_.push_back(new icarus::encounter::state(window_));
        break;
    case states::endgame:
        state_stack_.push_back(new icarus::endgame::state(window_));
        break;
    case states::options:
        state_stack_.push_back(new icarus::options::state(window_));
        break;
    }
}

void state_manager::push(states::state new_state)
{
    state_stack_.back()->pause_state();
    switch (new_state)
    {
    case states::intro:
    case states::menu:
        state_stack_.push_back(new icarus::menu::menu_state(window_));
        break;
    case states::overworld:
        state_stack_.push_back(new icarus::overworld::overworld_state(window_));
        break;
    case states::encounter:
        state_stack_.push_back(new icarus::encounter::state(window_));
        break;
    case states::endgame:
        state_stack_.push_back(new icarus::endgame::state(window_));
        break;
    case states::options:
        state_stack_.push_back(new icarus::options::state(window_));
        break;
    }
}

void state_manager::pop()
{
    delete state_stack_.back();
    state_stack_.pop_back();
    if (state_stack_.size() > 0)
        state_stack_.back()->resume_state();
}

void state_manager::quit()
{
    for (unsigned i = 0; i < state_stack_.size(); ++i)
    {
        delete state_stack_[i];
    }
    state_stack_.clear();
}

void state_manager::change_state(states::state new_state)
{
    state_event new_event;
    new_event.action_ = state_actions::change;
    new_event.state_ = new_state;
    state_events_.push_back(new_event);
}

void state_manager::push_state(states::state new_state)
{
    state_event new_event;
    new_event.action_ = state_actions::push;
    new_event.state_ = new_state;
    state_events_.push_back(new_event);
}

void state_manager::pop_state()
{
    state_event new_event;
    new_event.action_ = state_actions::pop;
    state_events_.push_back(new_event);
}

void state_manager::quit_game()
{
    state_event new_event;
    new_event.action_ = state_actions::quit;
    state_events_.push_back(new_event);
}

void state_manager::store_data(state_data* new_data)
{
    if (stored_data_)
        delete stored_data_;

    stored_data_ = new_data;
}

state_data* state_manager::retrieve_data()
{
    return stored_data_;
}

double state_manager::get_delta() const
{
    return static_cast<double>(delta_) * 0.000001;
}

void state_manager::set_cursor(const std::string& type)
{
    cursor_->set_animation_state(type);
    cursor_->update_anim();
}

void state_manager::reset_cursor_pos()
{
    cursor_->set_world_pos(input_handler::get()->get_mouse_xy<float>());
}

void state_manager::set_debug_profiler(double value)
{
    debug_data_.t_profiler_.setString(std::string("profiler: " + utils::to_str<double>(value) + "ms"));
}

void state_manager::save_game(base_state* state)
{
    if (state_stack_.size() == 0)
        return;
    std::ofstream stream;

    //TODO:: setup backup saves
    stream.open("savegames/autosave.dag", std::ios::out | std::ios::binary);
    uint8_t version = 0x2;
    char header[3] = {'D','A','G'};

    stream.write(reinterpret_cast<char*>(&version), sizeof(uint8_t));
    stream.write(header, sizeof(header));

    // save data
    if (state == NULL)
        state_stack_.back()->save(stream);
    else
        state->save(stream);

    stream.close();
}
void state_manager::load_game(base_state* state)
{
    if (!utils::file_exists("savegames/autosave.dag"))
        return;
    std::ifstream stream;
    stream.open("savegames/autosave.dag", std::ios::in | std::ios::binary);

    uint8_t version;
    char header[3];

    if (!stream.read(reinterpret_cast<char*>(&version), 1))
        std::cout << "error" << std::endl;
    if (!stream.read(reinterpret_cast<char*>(&header), 3))
        std::cout << "error" << std::endl;

    if (header[0] == 'D' && header[1] == 'A' && header[2] == 'G')
        std::cout << "Save file recognized as Icarus .DAG format, version " + utils::to_str(int(version)) << std::endl;

    // load data
    if (state == NULL)
        state_stack_.back()->load(stream);
    else
        state->load(stream);
    stream.close();
}

void state_manager::set_window_options(bool fullscreen, bool hd, bool vsync)
{
    if (fullscreen != is_fullscreen_ || hd != full_hd_)
    {
        window_->close();
        is_fullscreen_ = fullscreen;
        full_hd_ = hd;
        uint8_t fs = (fullscreen ? sf::Style::Fullscreen : 0);
        uint32_t s_width = (hd ? 1920 : 1280);
        uint32_t s_height = (hd ? 1080 : 720);
        window_->create(sf::VideoMode(s_width, s_height),
                        config_->get_value("window_title", "Encounter Prototype"),
                        fs);
        window_->setKeyRepeatEnabled(false);
        if (config_->get_value("frame_limit_enabled", false))
            window_->setFramerateLimit(config_->get_value("frame_limit", 60));
        window_->setVerticalSyncEnabled(vsync_enabled_);
        window_->setMouseCursorVisible(false);
        window_->setKeyRepeatEnabled(false);
    }
    if (vsync != vsync_enabled_)
    {
        window_->setVerticalSyncEnabled(vsync);
        vsync_enabled_ = vsync;
    }
}
}   // namespace icarus
