#ifndef HEX_SPRITE_H
#define HEX_SPRITE_H

#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "statistics.hpp"
#include "icarus/math/vector2.hpp"
namespace icarus
{
namespace overworld
{

class hex_sprite : public sf::Drawable
{
private:
    hex_sprite* this_hex_sprite;

    sf::Sprite hex_image_;
    sf::Sprite border_images_[6];
    sf::Sprite structure_image_;

    std::vector<hex_sprite*> surrounding_tiles;
    overworld::nation owned_by_nation;
    overworld::state type_occupation;
    overworld::nature type_nature;
    overworld::placement type_structure;

    unsigned type_ground;
    bool is_under_siege;

public:
    hex_sprite();
    virtual ~hex_sprite();
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

    void set_sprite_texture(std::vector<sf::Texture*> tex);
    void set_base_sprite(sf::Texture* tex);
    void set_sprite_coordinates(sf::Vector2i *cords, overworld::type_texture type);
    void set_surrounding_tile(hex_sprite* hex);

    void set_nation(const overworld::nation id);
    void set_nature(const unsigned id);
    void set_occupation(overworld::state id);
    void set_structure(overworld::placement id);
    void set_tile_coordinates(sf::Vector2f cords);
    void set_surrounding_occupied(overworld::state new_id);

    const overworld::nation get_nation();
    const overworld::nation get_const_nation() const;
    const unsigned          get_nature();
    const overworld::placement get_placement();
    const overworld::state  get_occupation() const;
    hex_sprite*             get_surrounding_tile(unsigned id);
    unsigned                get_surrounding_tile_length() const;
    const sf::Sprite*       get_sprite() const;

    sf::FloatRect           return_bounds() const;
    math::vector2f          get_world_pos() const;
    hex_sprite*             return_this_tile() const;
    hex_sprite*             return_this_tile_unconst();
    bool                    return_siege();
    void                    set_siege(bool is_sieged);

protected:

};
} // namespace overworld
} // namespace icarus

#endif // HEX_SPRITE_H
