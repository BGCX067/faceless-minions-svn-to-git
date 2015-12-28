

#include "icarus/menu/main.h"
#include "icarus/menu/main_menu.h"
#include "icarus/menu/main_buttons.h"
#include "icarus/base_state.hpp"


int main()
{
    //sf::RenderWindow window_(sf::VideoMode(1280, 720, 32), "Menu_test_01");
/*
    sf::Music music;
    music.openFromFile("music/main_theme.ogg");
    music.play();
    music.setLoop(true);
*/
    main_buttons *buttons;

    while (window_.isOpen())
    {
        sf::Event event;
        //sf::Mouse mouse;
        std::vector<sf::Event> mouse_events;
        while (window_.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window_.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                mouse_events.push_back(event);
            }

        }

        buttons->handle_input(mouse_events);

        window_.clear(sf::Color(100, 100, 50, 255));
        window_.draw(*buttons);
        window_.display();
    }
    return EXIT_SUCCESS;

}   //namespace icarus
