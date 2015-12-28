#ifndef CHARACTER_BASE_H
#define CHARACTER_BASE_H
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "character_base.hpp"
#include "animate.hpp"

class character_base: public animate
{
private:

protected:
    unsigned int sprite_width;
    unsigned int sprite_height;
    unsigned int sheet_width;
    unsigned int sheet_height;
    unsigned int sheet_column;
    unsigned int sheet_row;

public:
    character_base();
    character_base(unsigned sprite_height,
                   unsigned sprite_width,
                   unsigned sheet_height,
                   unsigned sheet_width);
    ~character_base();

    sf::Vector2f character_position_;
/*
    virtual void draw
    (sf::RenderTarget& target, sf::RenderStates states) const;
    //virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
*/
    //void animate();
};

#endif // CHARACTER_BASE_H
