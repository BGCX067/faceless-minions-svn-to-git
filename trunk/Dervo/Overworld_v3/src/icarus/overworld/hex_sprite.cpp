#include "icarus/overworld/hex_sprite.hpp"

namespace icarus
{
namespace overworld
{
hex_sprite::hex_sprite() :
                            this_hex_sprite(NULL),
                            owned_by_nation(overworld::nation::NEUTRAL_),
                            type_ground(3),
                            type_occupation(overworld::state::UNREACHABLE_),
                            is_under_siege(false)

{
    //cto
    this_hex_sprite = this;
    //owned_by_nation = overworld::nation::NEUTRAL_;
    is_under_siege = false;
    set_siege(false);
}

hex_sprite::~hex_sprite()
{
    //dtor
}
void hex_sprite::set_tile_coordinates(sf::Vector2f cords)
{
    hex_image_.setPosition(cords);
    for(unsigned border_tile =0; border_tile < 6;border_tile ++)
    {
        border_images_[border_tile].setPosition(cords);
    }
}

void hex_sprite::set_sprite_texture(std::vector<sf::Texture*> tex)
{
    hex_image_.setTexture(*tex[0]);
    for(unsigned border_tile =0; border_tile < 6;border_tile ++)
    {
        border_images_[border_tile].setTexture(*tex[1]);
    }
}
void hex_sprite::set_base_sprite(sf::Texture* tex)
{
    hex_image_.setTexture(*tex);
}
void hex_sprite::set_sprite_coordinates(sf::Vector2i *cords, overworld::type_texture temp)
{
    unsigned type = temp;
    if(type == 0)//overworld::type_texture::BASE_IMAGE
    {
        hex_image_.setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
    else if(type== 1)//overworld::type_texture::BORDER_0)
    {
        border_images_[0].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
    else if(type== 2)//overworld::type_texture::BORDER_1)
    {
        border_images_[1].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }

    else if(type== 3)//overworld::type_texture::BORDER_2)
    {
        border_images_[2].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
    else if(type== 4)//overworld::type_texture::BORDER_3)
    {
        border_images_[3].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
    else if(type== 5)//overworld::type_texture::BORDER_4)
    {
        border_images_[4].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
    else if(type== 6)//overworld::type_texture::BORDER_5)
    {
        border_images_[5].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
    else if(type== 7)//overworld::type_texture::STRUCTURE)
    {
        structure_image_.setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180));
    }
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
hex_sprite* hex_sprite::return_this_tile_unconst()
{
    return this_hex_sprite;
}

void hex_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(hex_image_, states);
    target.draw(border_images_[0], states);
    target.draw(border_images_[1], states);
    target.draw(border_images_[2], states);
    target.draw(border_images_[3], states);
    target.draw(border_images_[4], states);
    target.draw(border_images_[5], states);

}
void hex_sprite::set_nation(const overworld::nation id)
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
void hex_sprite::set_structure(overworld::placement id)
{
    type_structure = id;
}
const overworld::nation hex_sprite::get_nation()
{
    return owned_by_nation;
}
const overworld::nation hex_sprite::get_const_nation() const
{
    return owned_by_nation;
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
const overworld::placement hex_sprite::get_placement()
{
    return type_structure;
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

bool hex_sprite::return_siege()
{
    ////std::cout << "hex_sprite::return_siege" << is_under_siege << std::endl;
    return is_under_siege;
}
void hex_sprite::set_siege(bool is_sieged)
{
    is_under_siege = is_sieged;
}
} // namespace overworld
} // namespace icarus
