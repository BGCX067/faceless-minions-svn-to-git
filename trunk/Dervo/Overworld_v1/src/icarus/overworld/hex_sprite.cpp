#include "icarus/overworld/hex_sprite.hpp"

namespace icarus
{
namespace overworld
{
hex_sprite::hex_sprite() :  this_hex_sprite(NULL),
                            owned_by_nation(3),
                            type_ground(3),
                            type_occupation(overworld::state::UNREACHABLE_)

{
    //cto
    this_hex_sprite = this;
    owned_by_nation = 3;
}

hex_sprite::~hex_sprite()
{
    //dtor
}
void hex_sprite::set_tile_coordinates(sf::Vector2f cords)
{
    hex_image_.setPosition(cords);
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

hex_sprite* hex_sprite::get_surrounding_tile(unsigned id)
{
    return surrounding_tiles[id];
}
hex_sprite* hex_sprite::return_this_tile() const
{
    return this_hex_sprite;
}
void hex_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(hex_image_, states);

}
void hex_sprite::set_nation(const unsigned id)
{
    owned_by_nation = id;
}
void hex_sprite::set_nature(const unsigned id)
{
    type_ground = id;
}
void hex_sprite::set_occupation(overworld::state id)
{
    type_occupation = id;
}
const unsigned hex_sprite::get_nation()
{
    if(owned_by_nation)
    {
        return owned_by_nation;
    }
    else
    {
        return 3;
    }
}
const unsigned hex_sprite::get_nature()
{
    return type_ground;
}
const overworld::state hex_sprite::get_occupation() const
{
    return type_occupation;
}
unsigned hex_sprite::get_surrounding_tile_length() const
{
    return surrounding_tiles.size();
}

const sf::Sprite* hex_sprite::get_sprite() const
{
    return &hex_image_;
}
void hex_sprite::set_surrounding_occupied(overworld::state new_id)
{
    for(unsigned num_val = 0; num_val < surrounding_tiles.size(); num_val++)
    {
        if(surrounding_tiles[num_val]->get_occupation() != overworld::state::OCUPIED_)
        {
            surrounding_tiles[num_val]->set_occupation(new_id);
        }

    }
}
sf::FloatRect hex_sprite::return_bounds() const
{
    return hex_image_.getGlobalBounds();
}
math::vector2f hex_sprite::get_world_pos() const
{
    return hex_image_.getPosition();
}

} // namespace overworld
} // namespace icarus
