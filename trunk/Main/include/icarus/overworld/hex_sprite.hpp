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

    sf::Sprite base_tile_;
    sf::Sprite frac_border_[6];
    sf::Sprite nature_border_[6];
    sf::Sprite overlay_image_;

    std::vector<hex_sprite*> surrounding_tiles;

    overworld::type_frac            owned_by_nation;
    overworld::state                type_occupation;
    overworld::type_ground          base_color;
    overworld::type_overlay         base_overlay;
    float                           presence_mether;
    float                           importance_;

    unsigned attacking_nation;
    bool is_under_siege;
    bool is_used;

    statistics*                      stats;
    math::vector2i                  vector_position_;
    std::vector<sf::Vector2i>       border_coordinates;

public:
    hex_sprite(overworld::type_frac frac,
               overworld::type_ground ground,
               overworld::type_overlay overlay,
               overworld::state state,
                std::vector<sf::Vector2i>& border_coord);
    virtual ~hex_sprite();
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

    void update_prescence(float value)              {presence_mether += value;}
    void reset_presence()                           {presence_mether = 0;}
    void calculate_importance();
    void update_border(bool cont);

    void set_sprite_texture(std::vector<sf::Texture*> tex);
    void set_sprite_coordinates(sf::Vector2i *cords, overworld::type_texture type);
    void set_tile_coordinates(sf::Vector2f cords);
    const math::vector2i& get_vector_position() const   { return vector_position_; }
    void set_vector_position(const math::vector2i& pos) { vector_position_ = pos; }
    void set_surrounding_occupied(state new_id);
    void set_surrounding_tile(hex_sprite* hex)          {surrounding_tiles.push_back(hex);}
    void set_nation(const overworld::type_frac id)      {owned_by_nation = id;}
    void set_nature(const overworld::type_ground id)    {base_color = id;}
    void set_occupation(state id)                       {type_occupation = id;}
    void set_overlay(type_overlay id)                   {base_overlay = id;}
    void set_is_used(bool new_value)                    {is_used = new_value;}
    void set_siege(bool is_sieged)                      {is_under_siege = is_sieged;}
    void set_attacking_nation(unsigned i)               {attacking_nation = i;}

    const type_frac    get_nation()                     {return owned_by_nation;}
    const type_frac    get_const_nation() const         {return owned_by_nation;}
    const type_ground  get_nature()                     {return base_color;}
    const type_overlay get_overlay()                    {return base_overlay;}
    const type_overlay get_overlay()  const             {return base_overlay;}
    const state        get_occupation() const           {return type_occupation;}
    const sf::Sprite*  get_sprite() const               {return &base_tile_;}
    math::vector2f     get_world_pos() const            {return base_tile_.getPosition();}
    unsigned           get_surrounding_tile_length()    {return surrounding_tiles.size();}
    hex_sprite*        get_surrounding_tile(unsigned id){return surrounding_tiles[id];}
    bool               get_is_used()                    {return is_used;}
    float              get_importance()                 {return importance_;}
    unsigned           get_attacking_nation()           {return attacking_nation;}
    bool               get_override();

    sf::FloatRect      return_bounds() const            {return base_tile_.getGlobalBounds();}
    hex_sprite*        return_this_tile() const         {return this_hex_sprite;}
    hex_sprite*        return_this_tile_unconst()       {return this_hex_sprite;}
    bool               return_siege()                   {return is_under_siege;}
    float              return_presence()                {return presence_mether;}



protected:

};
} // namespace overworld
} // namespace icarus

#endif // HEX_SPRITE_H
