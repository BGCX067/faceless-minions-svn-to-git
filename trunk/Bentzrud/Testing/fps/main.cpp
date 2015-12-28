#include <iostream>
#include <SFML/Graphics.hpp>

void update_game()
{
    std::cout << "Updating game!" << std::endl;
}

void display_game(float interp)
{
    std::cout << "Interp is " << interp << std::endl;
}

int main()
{
    const int TICKS_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    const int MAX_FRAMESKIP = 5;
    sf::Clock clock;

    float next_game_tick = clock.getElapsedTime().asMilliseconds();
    int loops;
    float interpolation;

    bool game_is_running = true;
    while( game_is_running && clock.getElapsedTime().asSeconds() < 1.0f) {

        loops = 0;
        while( clock.getElapsedTime().asMilliseconds() > next_game_tick && loops < MAX_FRAMESKIP) {
            update_game();

            next_game_tick += SKIP_TICKS;
            loops++;
        }

        interpolation = float( clock.getElapsedTime().asMilliseconds() + SKIP_TICKS - next_game_tick )
                        / float( SKIP_TICKS );
        display_game( interpolation );
    }

    return 0;
}
