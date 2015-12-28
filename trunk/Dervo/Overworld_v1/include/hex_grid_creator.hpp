#ifndef HEX_GRID_CREATOR_H
#define HEX_GRID_CREATOR_H

#include "hex_sprite.hpp"
#include <string>
#include <ctime>
#include <iostream>


class hex_grid_creator
{
public:
    hex_grid_creator(unsigned int width, unsigned int height,
                     std::string text_url);
    virtual ~hex_grid_creator();
    //help function
    void change_variables(const int *x, const int *y);
    /*
    The function is meant to help sorting out numbers. used to find the
    different slots around
    */


    //stage1 - build grid
    void create_hex_grid(int num_x, int num_y);

    //stage2 - alocate friendly tiles
    void asign_nearby_tiles();
    hex_sprite* return_sprite(const unsigned col, const unsigned row);
    bool is_inside_bounds(const int row, const int col);


protected:
private:
    //hex_grid_creator* this_grid_creator;
    std::string texture_url_;
    typedef std::vector<hex_sprite> hex_hold_;
    std::vector<hex_hold_> hex_grid_;
    std::vector<sf::Vector2i> sprite_holder_;
    sf::Texture grid_texture_file_;
    unsigned int num_x_, num_y_;
    unsigned int texture_width, texture_height;

};

#endif // HEX_GRID_CREATOR_H
