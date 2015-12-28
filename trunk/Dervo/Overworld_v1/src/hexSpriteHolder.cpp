#include "../include/hexSpriteHolder.hpp"

hexSpriteHolder::hexSpriteHolder(sf::Texture *tex,  const sf::Vector2i& cords)
{
    //stuf
    hex_image.setTexture(*tex);
    const int& cord_x = cords.x;
    const int& cord_y = cords.y;
    hex_image.setTextureRect(sf::IntRect(cord_x, cord_y, 200, 280));

}

hexSpriteHolder::~hexSpriteHolder()
{
    //dtor

}
void hexSpriteHolder::set_position(int x_cord, int y_cord)
{
    hex_image.setPosition(x_cord, y_cord);
}
