#include <SFML/Graphics.hpp>
#include <iostream>
#include "simpledisplay.hpp"

simpledisplay::simpledisplay()
{
    std::cout << "Display up and running" << std::endl;

    //Graphics
    sf::RenderWindow window(sf::VideoMode(800, 600), "Icarus Rising");
    window.setFramerateLimit(60);

    //Testing
    sf::Texture spritetexture;
    spritetexture.loadFromFile("animation_mal_walk.png");

    character_hero heroman;

    heroman.character_sprite_ = new sf::Sprite(spritetexture);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            heroman.character_sprite_->move(10.0f, 0.0f);
            flip_sprite = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            heroman.character_sprite_->move(-10.0f, 0.0f);
            flip_sprite = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            heroman.character_sprite_->move(0.0f, -10.0f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            heroman.character_sprite_->move(0.0f, 10.0f);
        }

        window.clear();
        //heroman.animate();
        window.draw(heroman);
        window.display();
    }
}

simpledisplay::~simpledisplay()
{
    //dtor
}
