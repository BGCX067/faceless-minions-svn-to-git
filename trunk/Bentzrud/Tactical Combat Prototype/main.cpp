#include <iostream>
#include <ctime>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "icarus/encounter/state.hpp"
#include "icarus/input_handler.hpp"

int main()
{
    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Encounter Prototype");
    window.setFramerateLimit(60);

    icarus::input_handler::get_instance().setup_input(&window);
    icarus::encounter::state* state = new icarus::encounter::state(&window);

    icarus::encounter::encounter_data test_encounter;
    test_encounter.active_scenario = icarus::encounter::scenario::hills;
    if (!state->setup_encounter(test_encounter))
    {
        std::cout << "Setup error!" << std::endl;
        return 0;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        state->handle_input();

        state->handle_logic();

        window.clear();
        state->draw();
        window.display();
    }

    return 0;
}
