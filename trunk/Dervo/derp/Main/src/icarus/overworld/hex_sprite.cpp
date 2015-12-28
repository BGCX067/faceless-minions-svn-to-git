#include "icarus/overworld/hex_sprite.hpp"

namespace icarus
{
namespace overworld
{
hex_sprite::hex_sprite(
                overworld::type_frac frac,
                overworld::type_ground ground,
                overworld::type_overlay overlay,
                overworld::state state) :
                            owned_by_nation(frac),
                            type_occupation(state),
                            base_color(ground),
                            base_overlay(overlay),
                            is_under_siege(false),
                            is_used(false)
{
    this_hex_sprite = this;
}

hex_sprite::~hex_sprite()
{
    //dtor
}
void hex_sprite::set_tile_coordinates(sf::Vector2f cords)
{
    base_tile_.setPosition(cords);
    for(unsigned border_tile =0; border_tile < 6;border_tile ++)
    {
        frac_border_[border_tile].setPosition(cords);
        nature_border_[border_tile].setPosition(cords);
    }
    overlay_image_.setPosition(cords);
}
void hex_sprite::set_sprite_texture(std::vector<sf::Texture*> tex)
{
    base_tile_.setTexture(*tex[0]);
    for(unsigned border_tile =0; border_tile < 6;border_tile ++)
    {
        frac_border_[border_tile].setTexture(*tex[1]);
        nature_border_[border_tile].setTexture(*tex[1]);
    }
    overlay_image_.setTexture(*tex[2]);
}
void hex_sprite::set_sprite_coordinates(sf::Vector2i *cords, overworld::type_texture type)
{
    switch(type)
    {
        case BASE_IMAGE: base_tile_.setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case F_BORDER_0: frac_border_[0].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case F_BORDER_1: frac_border_[1].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case F_BORDER_2: frac_border_[2].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case F_BORDER_3: frac_border_[3].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case F_BORDER_4: frac_border_[4].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case F_BORDER_5: frac_border_[5].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case N_BORDER_0: nature_border_[0].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case N_BORDER_1: nature_border_[1].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case N_BORDER_2: nature_border_[2].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case N_BORDER_3: nature_border_[3].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case N_BORDER_4: nature_border_[4].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case N_BORDER_5: nature_border_[5].setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
        case OVERLAY_:   overlay_image_.setTextureRect(sf::IntRect(cords->x, cords->y, 180, 180)); break;
    }
}
void hex_sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(base_tile_, states);
    target.draw(nature_border_[0], states);
    target.draw(nature_border_[1], states);
    target.draw(nature_border_[2], states);
    target.draw(nature_border_[3], states);
    target.draw(nature_border_[4], states);
    target.draw(nature_border_[5], states);
    target.draw(frac_border_[0], states);
    target.draw(frac_border_[1], states);
    target.draw(frac_border_[2], states);
    target.draw(frac_border_[3], states);
    target.draw(frac_border_[4], states);
    target.draw(frac_border_[5], states);
    target.draw(overlay_image_, states);
}
void hex_sprite::set_surrounding_occupied(overworld::state new_id)
{
    for(unsigned num_val = 0; num_val < surrounding_tiles.size(); num_val++)
        if(surrounding_tiles[num_val]->get_occupation() != overworld::state::OCUPIED_)
            surrounding_tiles[num_val]->set_occupation(new_id);
}
/*void hex_sprite::add_structure(overworld::type_overlay type, std::vector<sf::Vector2i>& sprite, std::vector<unsigned> num_strucutres)
{
    int tile_nr = 0;
    if(type == overworld::type_overlay::TYPE_OVERLAY_NONE)
        tile_nr = overworld::sheet_overlays::so_empty;
    else
    {
        tile_nr = rand() % num_strucutres[type];
        tile_nr += (type*3);
    }
    set_sprite_coordinates(&sprite[tile_nr], overworld::type_texture::OVERLAY_);

    set_overlay(type);
}*/
bool hex_sprite::get_override()
{
    if(is_under_siege)
        return true;
    if(is_used)
        return true;
    return false;
}
} // namespace overworld
} // namespace icarus
