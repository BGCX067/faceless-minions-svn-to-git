#ifndef HEX_SPRITE_H
#define HEX_SPRITE_H

#include <vector>
#include <SFML/Graphics.hpp>

class hex_sprite : public sf::Drawable
{
public:
    hex_sprite();
    virtual ~hex_sprite();

    void set_sprite_texture(sf::Texture *tex);
    void set_sprite_coordinates(sf::Vector2i *cords);
    void set_tile_id(unsigned int id);
    unsigned int get_tile_id();
    void set_surrounding_tile(hex_sprite* hex);
    hex_sprite* get_surrounding_tile(unsigned int id);
    hex_sprite* return_this_tile();
    void set_tile_coordinates(sf::Vector2f cords);
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
protected:
private:
    sf::Vector2i tile_coordinates;
    hex_sprite* this_hex_sprite;
    sf::Sprite hex_image_;
    unsigned int tile_id;
    std::vector<hex_sprite*> surrounding_tiles;

};

#endif // HEX_SPRITE_H
