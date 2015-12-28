#include <ctime>
#include "icarus/state_manager.hpp"
#include "icarus/utilities.hpp"

namespace icarus
{
state_manager::state_manager() :    delta_(0),
                                    prev_delta_(0),
                                    stored_data_(NULL),
                                    window_(NULL)
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
        }
        // screenshot hook
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::F9)
            take_screenshot();

        state_stack_.back()->handle_input(key_events);

        state_stack_.back()->handle_logic();

        window_->clear();
        state_stack_.back()->draw();
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
    std::string timestring =    utilities::to_str(timeinfo->tm_mday, 2) +
                                utilities::to_str(timeinfo->tm_mon + 1, 2) +
                                utilities::to_str(timeinfo->tm_year + 1900);
    std::string filename = "";
    int counter = 0;
    do
    {
        filename = std::string("screenshots/vanadia_ss_") +
                    timestring + "_" +
                    utilities::to_str(counter, 3) +
                    ".png";
        counter++;
    } while (utilities::file_exists(filename));
    window_->capture().saveToFile(filename);
}

void state_manager::start(base_state* initial_state, sf::RenderWindow* window)
{
    //config_.load_config_file("settings.cfg");
    window_ = window;
    window_->setKeyRepeatEnabled(false);
    state_stack_.push_back(initial_state);
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
}   // namespace icarus
