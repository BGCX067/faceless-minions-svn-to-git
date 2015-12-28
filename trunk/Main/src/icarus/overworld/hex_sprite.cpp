#include "icarus/overworld/hex_sprite.hpp"

namespace icarus
{
namespace overworld
{
hex_sprite::hex_sprite(
                overworld::type_frac frac,
                overworld::type_ground ground,
                overworld::type_overlay overlay,
                overworld::state state,
                std::vector<sf::Vector2i>& border_coord) :
                            owned_by_nation(frac),
                            type_occupation(state),
                            base_color(ground),
                            base_overlay(overlay),
                            attacking_nation(4),
                            is_under_siege(false),
                            is_used(false),
                            stats(statistics::get()),
                            border_coordinates(border_coord)
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
    for(unsigned border_this_hex_sprite =0; border_this_hex_sprite < 6;border_this_hex_sprite ++)
    {
        frac_border_[border_this_hex_sprite].setPosition(cords);
        nature_border_[border_this_hex_sprite].setPosition(cords);
    }
    overlay_image_.setPosition(cords);
}
void hex_sprite::set_sprite_texture(std::vector<sf::Texture*> tex)
{
    base_tile_.setTexture(*tex[0]);
    for(unsigned border_this_hex_sprite =0; border_this_hex_sprite < 6;border_this_hex_sprite ++)
    {
        frac_border_[border_this_hex_sprite].setTexture(*tex[1]);
        nature_border_[border_this_hex_sprite].setTexture(*tex[1]);
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
    int this_hex_sprite_nr = 0;
    if(type == overworld::type_overlay::TYPE_OVERLAY_NONE)
        this_hex_sprite_nr = overworld::sheet_overlays::so_empty;
    else
    {
        this_hex_sprite_nr = rand() % num_strucutres[type];
        this_hex_sprite_nr += (type*3);
    }
    set_sprite_coordinates(&sprite[this_hex_sprite_nr], overworld::type_texture::OVERLAY_);

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
void hex_sprite::calculate_importance()
{
    importance_ =0;
    for(unsigned i=0;i<surrounding_tiles.size();i++)
        if(surrounding_tiles[i]->get_nation() != get_nation())
            importance_ ++;
    importance_ -= stats->local_presense[get_overlay()];
    if(importance_ < 1)
        importance_ =1;
}
void hex_sprite::update_border(bool cont)
{
    for(unsigned border_sprite =0;border_sprite < surrounding_tiles.size();border_sprite ++)
    {
        math::vector2f this_coord = get_world_pos();
        math::vector2f other_coord = surrounding_tiles[border_sprite]->get_world_pos();

        overworld::type_texture nation_layer;
        unsigned                nation_value = 6 * static_cast<unsigned>(owned_by_nation);
        if(this_coord.x > other_coord.x && this_coord.y > other_coord.y)
        {
            nation_layer = overworld::type_texture::F_BORDER_0;
            nation_value += 4;
        }
        else if(this_coord.x < other_coord.x && this_coord.y > other_coord.y)
        {
            nation_layer = overworld::type_texture::F_BORDER_1;
            nation_value += 3;
        }
        else if(this_coord.x > other_coord.x && this_coord.y == other_coord.y)
        {
            nation_layer = overworld::type_texture::F_BORDER_2;
            nation_value += 5;
        }
        else if(this_coord.x < other_coord.x && this_coord.y == other_coord.y)
        {
            nation_layer = overworld::type_texture::F_BORDER_3;
            nation_value += 2;
        }
        else if(this_coord.x > other_coord.x && this_coord.y < other_coord.y)
        {
            nation_layer = overworld::type_texture::F_BORDER_4;
            nation_value += 0;
        }
        else if(this_coord.x < other_coord.x && this_coord.y < other_coord.y)
        {
            nation_layer = overworld::type_texture::F_BORDER_5;
            nation_value += 1;
        }
        else
        {
            std::cout << "Something is horrible wrong in HEX_SPRITE::UPDATE_BORDER"<< std::endl;
        }
        sf::Vector2i cords;
        if(owned_by_nation != surrounding_tiles[border_sprite]->get_nation() && owned_by_nation < 3)
            cords = border_coordinates[nation_value];
        else
            cords  = border_coordinates[sheet_border_none];

        set_sprite_coordinates(&cords, nation_layer);

        if(cont)
            surrounding_tiles[border_sprite]->update_border(false);
    }
}
} // namespace overworld
} // namespace icarus
