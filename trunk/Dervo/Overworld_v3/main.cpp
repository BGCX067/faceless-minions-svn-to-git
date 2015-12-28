
#include <SFML/Graphics.hpp>
#include <vector>
#include "icarus/overworld/overworld_state.hpp"
#include "icarus/input_handler.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    srand (time (NULL));

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Overworld prototype");
    window.setFramerateLimit(60);

    //icarus::input_handler::get_instance().setup_input(&window);
    icarus::overworld::overworld_state* overworld_ =
                    new icarus::overworld::overworld_state(&window);

    overworld_->setup_overworld();

    while (window.isOpen())
    {
        sf::Event event;
        std::vector<sf::Event> key_events;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed ||
                event.type == sf::Event::KeyReleased ||
                event.type == sf::Event::TextEntered ||
                event.type == sf::Event::MouseButtonPressed ||
                event.type == sf::Event::MouseButtonReleased)
            {
                key_events.push_back(event);
            }
        }
        overworld_->handle_input(key_events);
        overworld_->handle_logic();

        window.clear();
        overworld_->draw();

        window.display();
    }
    return 0;
}
