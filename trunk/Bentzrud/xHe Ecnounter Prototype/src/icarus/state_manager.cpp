#include <ctime>
#include "icarus/state_manager.hpp"
#include "icarus/utils.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
state_manager* state_manager::instance_ = NULL;

state_manager::state_manager() :    delta_(0),
                                    prev_delta_(0),
                                    stored_data_(NULL),
                                    window_(NULL),
                                    debug_flags_(0x0),
                                    cursor_(NULL)
{
}

state_manager::~state_manager()
{
}

void state_manager::run_game()
{
    while (window_->isOpen())
    {
        if (state_stack_.size() == 0)
            return;

        sf::Event event;
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
                take_screenshot();
            else if (event.type == sf::Event::KeyPressed)
                update_debug_flags(event);
            else if (event.type == sf::Event::MouseMoved)
                cursor_->set_world_pos(math::vector2f(window_->convertCoords(sf::Mouse::getPosition(*window_))));
        }

        state_stack_.back()->handle_input(key_events);

        state_stack_.back()->handle_logic();

        window_->clear();
        state_stack_.back()->draw();
        window_->draw(*cursor_);
        draw_debug();
        window_->display();
        delta_ = game_time_.getElapsedTime().asMicroseconds() - prev_delta_;
        prev_delta_ = game_time_.getElapsedTime().asMicroseconds();
    }
}

void state_manager::take_screenshot()
{
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
}

void state_manager::update_debug_flags(const sf::Event& event)
{
    if (event.key.code == sf::Keyboard::F && event.key.control)
        debug_flags_ = (debug_flags_ & debug::SHOW_FPS ?
                        debug_flags_ & ~debug::SHOW_FPS :
                        debug_flags_ | debug::SHOW_FPS);
}

void state_manager::draw_debug()
{
    if (debug_flags_ & debug::SHOW_FPS)
    {
        sf::Text fps(std::string("fps: " + utils::to_str<int>(1000000/delta_)));
        fps.setCharacterSize(10);
        fps.setPosition(window_->convertCoords(sf::Vector2i(0.0f, window_->getSize().y - fps.getLocalBounds().height)));
        window_->draw(fps);
    }
}

void state_manager::initialize(const std::string& config_path)
{
    config_.load_config_file(config_path);

    window_ = new sf::RenderWindow(sf::VideoMode(config_.get_value("resolution_width", 1280),
                                           config_.get_value("resolution_height", 720)),
                                           config_.get_value("window_title", "Encounter Prototype"));
    if (config_.get_value("frame_limit_enabled", false))
        window_->setFramerateLimit(config_.get_value("frame_limit", 60));

    icarus::input_handler::get()->setup_input(window_);
    window_->setKeyRepeatEnabled(false);

    int initial_state = config_.get_value("initial_state",
                                          static_cast<int>(states::encounter));

    yth_handler_.load(config_.get_value("resource_index", "resources/index.yth"));
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

void state_manager::start()
{
    switch (initial_state_)
    {
    case states::intro:
    case states::menu:
    case states::overworld:
    case states::encounter:
        state_stack_.push_back(new icarus::encounter::state(window_));
        break;
    }
    game_time_.restart();
    run_game();
}

void state_manager::change_state(base_state* new_state)
{
    delete state_stack_.back();
    state_stack_.pop_back();
    state_stack_.push_back(new_state);
}

void state_manager::push_state(base_state* new_state)
{
    state_stack_.back()->pause_state();
    state_stack_.push_back(new_state);
}

void state_manager::pop_state()
{
    delete state_stack_.back();
    state_stack_.pop_back();
    if (state_stack_.size() > 0)
        state_stack_.back()->resume_state();
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
}   // namespace icarus
