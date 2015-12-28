#ifndef HEXSPRITEHOLDER_H
#define HEXSPRITEHOLDER_H
#include <iostream>
#include <SFML/Graphics.hpp>

class hexSpriteHolder
{
public:
    hexSpriteHolder(sf::Texture *tex, const sf::Vector2i& cords);
    virtual ~hexSpriteHolder();

    int coordinat_x;
    int coordinat_y;
    sf::Sprite hex_image;

    void set_position(int x_cord, int y_cord);
protected:
private:
};

#endif // HEXSPRITEHOLDER_H
