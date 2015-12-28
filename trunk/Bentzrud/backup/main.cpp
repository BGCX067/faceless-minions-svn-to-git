#include <iostream>
#include <ctime>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "icarus/encounter/state.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/input_handler.hpp"

int main()
{
    srand(0xabcdef);

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Encounter Prototype");
    //window.setFramerateLimit(60);

    icarus::input_handler::get_instance().setup_input(&window);
    icarus::encounter::data test_encounter(icarus::encounter::scenario::hills);
    icarus::state_manager::get_instance().store_data(&test_encounter);
    icarus::state_manager::get_instance().start(new icarus::encounter::state(&window),
                                                &window);

    return 0;
}
