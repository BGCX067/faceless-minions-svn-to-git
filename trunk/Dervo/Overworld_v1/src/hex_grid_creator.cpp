#include "../include/hex_grid_creator.hpp"

hex_grid_creator::hex_grid_creator(unsigned int width,
                                   unsigned int height,
                                   std::string text_url) :
                                       texture_url_(text_url),
                                       texture_height(height),
                                        texture_width(width)
{
    //ctor
    //create tiles;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            sf::Vector2i cord(j*texture_height, i*texture_width);
            sprite_holder_.push_back(cord);
        }
    }
    grid_texture_file_.loadFromFile(texture_url_);
}

hex_grid_creator::~hex_grid_creator()
{
    //dtor
}
void hex_grid_creator::create_hex_grid(int num_x = 10, int num_y = 10)
{
    num_x_ = num_x;
    num_y_ = num_y;
    srand (time (NULL));
    //asign sprite texture and coordiantes
    for(int row = 0; row < num_x_; row++)
    {
        std::vector<hex_sprite> vec2;
        for(int col = 0; col < num_y_; col++)
        {
            int random_number = rand() % sprite_holder_.size() + 1;
            hex_sprite hex_class;
            sf::Vector2i sprite_type = sprite_holder_[random_number];
            hex_class.set_sprite_texture(&grid_texture_file_);
            hex_class.set_sprite_coordinates(&sprite_type);

            sf::Vector2f cords;
            //set position
            cords.y = col * (texture_height - 99);//197;
            cords.x = row * texture_width;//173;

            int j = col;
            while(j > 1) { j -=2 ; }
            if(j==1){ cords.x += (texture_width / 2); }

            hex_class.set_tile_coordinates(cords);

            vec2.push_back(hex_class);
        }
    hex_grid_.push_back(vec2);
    }
}
bool is_inside_bounds(const int row, const int col)
{
    if(!(row < 0) && !(row > hex_grid_[0].size()) &&
       !(col < 0) && !(col > hex_grid_[0].size()))
    {
        return true;
    }
    else
    {
        return false;
    }

}
void hex_grid_creator::change_variables(const int *num_width, const int *num_height)
{
    int x;
    int y;
    for(int i = 0; i < 6; i++)
    {
        int is_one_or_zero = y;
        while( is_one_or_zero > 1) { is_one_or_zero -=2;}

        //**********************************************************
        //**********************************************************
        //** Change this
        //**********************************************************
        //**********************************************************


        switch(i)
        {
        case 0:
            y = *num_height -1;
            x = *num_width - is_one_or_zero;'
            if(!is_inside_bounds(*num_width , num_height))
        break;
        case 1:
            y = *num_height -1;
            x = *num_width +1 - is_one_or_zero;
        break;
        case 2:
            y = *num_height;
            x = *num_width -1;
        break;
        case 3:
            y = *num_height;
            x = *num_width +1;
        break;
        case 4:
            y = *num_height +1;
            x = *num_width - is_one_or_zero;
        break;
        case 5:
            y = *num_height +1;
            x = *num_width + 1 - is_one_or_zero;
        break;
        }
        std::cout << "Stuff of legends, x:" << *num_width + x << " and y:" << *num_height + y << std::endl;
        hex_grid_[*num_width][*num_height].set_surrounding_tile(
                        hex_grid_[*num_width + x][*num_height + y].return_this_tile());
    }
}
void hex_grid_creator::asign_nearby_tiles()
{
    for(int col = 0; col < num_y_; col++)
    {
        for(int row = 0; row < num_x_; row++)
        {
            std::cout << "Satan" << std::endl;
            change_variables(&row, &col);
        }
    }
}
hex_sprite* hex_grid_creator::return_sprite(const unsigned int col, const unsigned int row)
{
    return &hex_grid_[col][row];
}
