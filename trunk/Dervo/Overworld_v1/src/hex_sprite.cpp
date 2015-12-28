#include "../include/hex_sprite.hpp"

hex_sprite::hex_sprite()
{
    //cto
    this_hex_sprite = this;
}

hex_sprite::~hex_sprite()
{
    //dtor
}
void hex_sprite::set_tile_coordinates(sf::Vector2f cords)
{
    hex_image_.setPosition(cords);
}

void hex_sprite::set_tile_id(unsigned int id)
{
    tile_id = id;
}

unsigned int hex_sprite::get_tile_id()
{
    return tile_id;
}

void hex_sprite::set_sprite_texture(sf::Texture *tex)
{
    hex_image_.setTexture(*tex);
}

void hex_sprite::set_sprite_coordinates(sf::Vector2i *cords)
{
    hex_image_.setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
}

void hex_sprite::set_surrounding_tile(hex_sprite *hex)
{
    surrounding_tiles.push_back(hex);
}

hex_sprite* hex_sprite::get_surrounding_tile(unsigned int id)
{
    return surrounding_tiles[id];
}
hex_sprite* hex_sprite::return_this_tile()
{
    return this_hex_sprite;
}
void hex_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(hex_image_, states);
}

