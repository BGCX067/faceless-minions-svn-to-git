#include "icarus/overworld/hex_grid_creator.hpp"
namespace icarus
{
namespace overworld
{
hex_grid_creator::hex_grid_creator() : spawn_point_(NULL)
{
    for(int i = 0; i <= 4; i++)
    {
        for(int k = 1; k <= 6; k++)
        {
            sf::Vector2i cord1(k*stats.pixel_height_, i*stats.pixel_width_);
            sprite_border_.push_back(cord1);
        }
        for(int j = 1; j < 10; j++)
        {
            sf::Vector2i cord2(j*stats.pixel_height_, i*stats.pixel_width_);

            // overworld_natur
            sprite_nature_.push_back(cord2);
            sprite_factions_.push_back(cord2);
            sprite_structures_.push_back(cord2);
        }
    }
    texture_nature_.loadFromFile(stats.url_nature_);
    texture_border_.loadFromFile(stats.url_border_);
    texture_factions_.loadFromFile(stats.url_factions_);
    texture_structures_.loadFromFile(stats.url_structures_);
}

hex_grid_creator::~hex_grid_creator()
{
    //dtor
}
void hex_grid_creator::create_hex_grid(std::vector<hex_hold_>& hex_grid_)
{
    //asign sprite texture and coordiantes
    for(unsigned col = 0; col < stats.grid_height_; col++)
    {
        std::vector<hex_sprite> vec2;
        for(unsigned row = 0; row < stats.grid_width_; row++)
        {
            hex_sprite hex_class;
            hex_class.set_nation(overworld::nation::NEUTRAL_);
            hex_class.set_nature(overworld::nature::FORREST_);
            std::vector<sf::Texture*> temp;
            temp.push_back(&texture_factions_);
            temp.push_back(&texture_border_);
            temp.push_back(&texture_nature_);
            temp.push_back(&texture_structures_);
            hex_class.set_sprite_texture(temp);
            sf::Vector2i sprite_type =
                        sprite_factions_[overworld::sheet_fraction::NEUTRAL_PLANE];//0
            hex_class.set_sprite_coordinates(&sprite_type,
                                        overworld::type_texture::BASE_IMAGE);

            sf::Vector2f cords;
            //set position
            cords.y = col * (stats.pixel_height_ - stats.hex_offset_);//197;
            cords.x = row * stats.pixel_width_;//173;

            int j = col%2;
            if(j==1){ cords.x += (stats.pixel_width_ / 2); }

            hex_class.set_tile_coordinates(cords);

            vec2.push_back(hex_class);
        }
    hex_grid_.push_back(vec2);
    }
}
bool hex_grid_creator::is_inside_bounds(const unsigned row, const unsigned col)
{
    if(!(col < 0) && !(col > (stats.grid_height_ -1)) &&
       !(row < 0) && !(row > (stats.grid_width_ -1)))
    {
        return true;
    }
    else
    {
        return false;
    }

}
void hex_grid_creator::change_variables(std::vector<hex_hold_>& hex_grid_,
                                        const unsigned *num_width,
                                        const unsigned *num_height)
{
    int x;
    int y;
    int is_one_or_zero = (*num_height +1) %2;
    for(int i = 0; i < 6; i++)
    {
        switch(i)
        {
        case 0:
            y = *num_height -1;
            x = *num_width - is_one_or_zero;
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
        if(is_inside_bounds(x , y))
        {
            hex_grid_[*num_height][*num_width].set_surrounding_tile(
                            &hex_grid_[y][x]);
        }
    }
}
void hex_grid_creator::asign_nearby_tiles(std::vector<hex_hold_>& hex_grid_)
{
    for(unsigned col = 0; col < stats.grid_height_; col++)
    {
        for(unsigned row = 0; row < stats.grid_width_; row++)
        {
            change_variables(hex_grid_, &row, &col);
        }
    }
}

hex_sprite* hex_grid_creator::return_sprite(std::vector<hex_hold_>& hex_grid_,
                                            const unsigned row,
                                            const unsigned col)
{
    return &hex_grid_[col][row];
}
const hex_sprite* hex_grid_creator::return_const_sprite(
                                            const std::vector<hex_hold_>& hex_grid_,
                                            const unsigned row,
                                            const unsigned col) const
{
    return &hex_grid_[col][row];
}

const sf::FloatRect hex_grid_creator::pass_bounds(
                                        std::vector<hex_hold_>& hex_grid_,
                                        const unsigned row,
                                        const unsigned col) const
{
    sf::FloatRect temp = hex_grid_[col][row].return_bounds();
    temp.top += stats.hex_sprite_margin;
    temp.height -= stats.hex_sprite_margin;
    return temp;
}

void hex_grid_creator::generate_starter_fractions(
                                    std::vector<hex_hold_>& hex_grid_,
                                    std::vector<nation_group_>& nation_tiles_,
                                    const unsigned num_frac,
                                    const double percentage_uncaptured)
{
    double total_tiles = stats.grid_height_ * stats.grid_width_;
    total_tiles *= 1 - percentage_uncaptured;
    const unsigned num_aviable_tiles = static_cast<int>(total_tiles);
    const unsigned num_tiles_per_nation = num_aviable_tiles / num_frac;

    // Generate the 8 startingpositions.
    const unsigned col = stats.grid_height_ -1;
    const unsigned row = stats.grid_width_ -1;
    unsigned half_col = col/2;
    unsigned half_row = row/2;

    int i = col % 2;
    if(i == 1){ half_col ++;}
    else{       half_col += rand() % 1;}

    i = (row +1 ) % 2;
    if(i == 1){ half_row ++;}
    else{       half_row += rand() % 1;}

    std::vector<hex_sprite*> spawn_tiles;
    spawn_tiles.push_back(&hex_grid_[1][1]);
    spawn_tiles.push_back(&hex_grid_[1][row-1]);
    spawn_tiles.push_back(&hex_grid_[col-1][1]);
    spawn_tiles.push_back(&hex_grid_[col-1][row-1]);
    spawn_tiles.push_back(&hex_grid_[1][half_row]);
    spawn_tiles.push_back(&hex_grid_[col-1][half_row]);
    spawn_tiles.push_back(&hex_grid_[half_col][1]);
    spawn_tiles.push_back(&hex_grid_[half_col][row-1]);

    nation_group_ n_group;

    // chose starting positions based on the number of "num_frac"
    for(unsigned i = 0; i < num_frac; i++)
    {
        const unsigned choosen_tile = rand() % spawn_tiles.size();
        switch (i)
        {
            case 0:
                spawn_tiles[choosen_tile]->set_nation(overworld::nation::RED_);
                spawn_tiles[choosen_tile]->set_sprite_coordinates(
                        &sprite_factions_[overworld::sheet_fraction::RED_PLANE],
                        overworld::type_texture::BASE_IMAGE);
                break;
            case 1:
                spawn_tiles[choosen_tile]->set_nation(overworld::nation::GREEN_);
                spawn_tiles[choosen_tile]->set_sprite_coordinates(
                        &sprite_factions_[overworld::sheet_fraction::GREEN_PLANE],
                        overworld::type_texture::BASE_IMAGE);
                break;
            case 2:
                spawn_tiles[choosen_tile]->set_nation(overworld::nation::BLUE_);
                spawn_tiles[choosen_tile]->set_sprite_coordinates(
                        &sprite_factions_[overworld::sheet_fraction::BLUE_PLANE],
                        overworld::type_texture::BASE_IMAGE);
                break;
            case 3:
                spawn_tiles[choosen_tile]->set_nation(overworld::nation::SPAWN_);
                spawn_tiles[choosen_tile]->set_sprite_coordinates(
                        &sprite_factions_[overworld::sheet_fraction::SPAWN_PLANE],
                        overworld::type_texture::BASE_IMAGE);
                break;
        }

        n_group.push_back(spawn_tiles[choosen_tile]);
        nation_tiles_.push_back(n_group);
        n_group.clear();
        spawn_tiles.erase (spawn_tiles.begin()+choosen_tile);
        std::cout << "num_tiles_per_nation " << num_tiles_per_nation -1 <<std::endl;
    }
    for(unsigned num_tiles = 0; num_tiles < num_tiles_per_nation -1; num_tiles++)
    {
        for(unsigned per_frac = 0; per_frac < num_frac; per_frac ++)
        {
            overworld::nation frac;
            switch (per_frac)
            {
            case 0: frac = overworld::nation::RED_;break;
            case 1: frac = overworld::nation::GREEN_;break;
            case 2: frac = overworld::nation::BLUE_;break;
            case 3: frac = overworld::nation::SPAWN_;break;
            }
           change_nearby_tile_to_same(hex_grid_, nation_tiles_[frac], frac, overworld::nation::NEUTRAL_, true);
        }
    }
}
void hex_grid_creator::set_tile_to(const hex_sprite* active_tile,
                                   const unsigned id)
{

}

void hex_grid_creator::change_nearby_tile_to_same(
                                    std::vector<hex_hold_>& hex_grid_,
                                    nation_group_& group,
                                    const overworld::nation type_tile,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border)
{
    std::vector<hex_sprite*> usable_tiles;
    for(unsigned per_member =0; per_member < group.size(); per_member++)
    {
        std::vector<hex_sprite*> surrounding_tiles;
        for(unsigned vector_length = 0; vector_length <
        group[per_member]->get_surrounding_tile_length(); vector_length ++)
        {
            if(surrounding_tiles.size() < max_border)
            {
                if(is_nation && static_cast<unsigned>(group[per_member]->
                                    get_surrounding_tile(vector_length)->
                                    get_nation()) == base)
                {
                    surrounding_tiles.push_back(group[per_member]->
                                       get_surrounding_tile(vector_length));
                }
                if(!is_nation && group[per_member]->
                                    get_surrounding_tile(vector_length)->
                                    get_nature() == base)
                {
                    surrounding_tiles.push_back(group[per_member]->
                                        get_surrounding_tile(vector_length));
                }
            }
            usable_tiles.insert( usable_tiles.end(),
                                surrounding_tiles.begin(),
                                surrounding_tiles.end() );
            if(surrounding_tiles.size() > 0)
                surrounding_tiles.clear();
        }
    }
    if(usable_tiles.size() > 0)
    {
        const unsigned scelected_tile =  rand() % usable_tiles.size();
        sf::Vector2i sprite_type;
        if(is_nation)
        {
            usable_tiles[scelected_tile]->set_nation(type_tile);
            group.push_back(usable_tiles[scelected_tile]);
            unsigned type;
            switch (usable_tiles[scelected_tile]->
                        get_nation())
            {//RED_, BLUE_, GREEN_, SPAWN_, NEUTRAL_
            case 0: type = overworld::sheet_fraction::RED_PLANE;break;
            case 1: type = overworld::sheet_fraction::GREEN_PLANE;break;
            case 2: type = overworld::sheet_fraction::BLUE_PLANE;break;
            case 3: type = overworld::sheet_fraction::SPAWN_PLANE;break;
            }
            sprite_type = sprite_factions_[type];
            usable_tiles[scelected_tile]->set_base_sprite(&texture_factions_);
            usable_tiles[scelected_tile]->set_sprite_coordinates(&sprite_type,
                        overworld::type_texture::BASE_IMAGE);
        }
        else
        {
            usable_tiles[scelected_tile]->set_nature(type_tile);
            group.push_back(usable_tiles[scelected_tile]);
            sprite_type = sprite_nature_[usable_tiles[scelected_tile]->
                        get_nature()];
            usable_tiles[scelected_tile]->set_base_sprite(&texture_nature_);
            usable_tiles[scelected_tile]->set_sprite_coordinates(&sprite_type,
                        overworld::type_texture::BASE_IMAGE);
        }
    }
}
void hex_grid_creator::change_nearby_tile_to_same(
                                    std::vector<hex_hold_>& hex_grid_,
                                    nation_group_& group,
                                    const unsigned type_tile,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border)
{
    std::vector<hex_sprite*> usable_tiles;
    for(unsigned per_member =0; per_member < group.size(); per_member++)
    {
        std::vector<hex_sprite*> surrounding_tiles;
        for(unsigned vector_length = 0; vector_length <
        group[per_member]->get_surrounding_tile_length(); vector_length ++)
        {
            if(surrounding_tiles.size() < max_border)
            {
                if(is_nation && static_cast<unsigned>(group[per_member]->
                                    get_surrounding_tile(vector_length)->
                                    get_nation()) == base)
                {
                    surrounding_tiles.push_back(group[per_member]->
                                       get_surrounding_tile(vector_length));
                }
                if(!is_nation && group[per_member]->
                                    get_surrounding_tile(vector_length)->
                                    get_nature() == base)
                {
                    surrounding_tiles.push_back(group[per_member]->
                                        get_surrounding_tile(vector_length));
                }
            }
            usable_tiles.insert( usable_tiles.end(),
                                surrounding_tiles.begin(),
                                surrounding_tiles.end() );
            if(surrounding_tiles.size() > 0)
                surrounding_tiles.clear();
        }
    }
    if(usable_tiles.size() > 0)
    {
        overworld::nation frac;
        switch (type_tile)
        {
        case 0: frac = overworld::nation::RED_;break;
        case 1: frac = overworld::nation::GREEN_;break;
        case 2: frac = overworld::nation::BLUE_;break;
        case 3: frac = overworld::nation::NEUTRAL_;break;
        }
        const unsigned scelected_tile =  rand() % usable_tiles.size();
        sf::Vector2i sprite_type;
        if(is_nation)
        {
            usable_tiles[scelected_tile]->set_nation(frac);
            group.push_back(usable_tiles[scelected_tile]);
            unsigned type;
            switch (usable_tiles[scelected_tile]->
                        get_nation())
            {//RED_, BLUE_, GREEN_, SPAWN_, NEUTRAL_
            case 0: type = overworld::sheet_fraction::RED_PLANE;break;
            case 1: type = overworld::sheet_fraction::GREEN_PLANE;break;
            case 2: type = overworld::sheet_fraction::BLUE_PLANE;break;
            case 3: type = overworld::sheet_fraction::SPAWN_PLANE;break;
            }
            sprite_type = sprite_factions_[type];
            usable_tiles[scelected_tile]->set_base_sprite(&texture_nature_);
            usable_tiles[scelected_tile]->set_sprite_coordinates(&sprite_type,
                        overworld::type_texture::BASE_IMAGE);
        }
        else
        {

            usable_tiles[scelected_tile]->set_nature(type_tile);
            group.push_back(usable_tiles[scelected_tile]);
            sprite_type = sprite_factions_[usable_tiles[scelected_tile]->
                        get_nature()];
            usable_tiles[scelected_tile]->set_base_sprite(&texture_nature_);
            usable_tiles[scelected_tile]->set_sprite_coordinates(&sprite_type,
                        overworld::type_texture::BASE_IMAGE);
        }
    }
}
void hex_grid_creator::generate_nature_tiles(
                                    std::vector<hex_hold_>& hex_grid_,
                                    std::vector<nation_group_>& nature_tiles_,
                                    const unsigned tile_id,
                                    unsigned number_of_spawn_tiles,
                                    const double percentage_beeing_this,
                                    const unsigned max_bordering,
                                    const bool require_border)
{
    double total_tiles = stats.grid_height_ * stats.grid_width_;
    total_tiles *= percentage_beeing_this;
    const unsigned num_aviable_tiles = static_cast<int>(total_tiles);

    std::vector<hex_sprite*> spawn_tiles;

    if(num_aviable_tiles < number_of_spawn_tiles)
        number_of_spawn_tiles = num_aviable_tiles;

    //Adds mountain spawn points.
    for(unsigned num_spawn =0; num_spawn < number_of_spawn_tiles;num_spawn ++)
    {
        unsigned col;
        unsigned row;
        do
        {
            col = rand() % (stats.grid_height_ -1);
            row = rand() % (stats.grid_width_ -1);
        }
        while(hex_grid_[col][row].get_nature() != overworld::nature::FORREST_);
        spawn_tiles.push_back(&hex_grid_[col][row]);

        spawn_tiles[num_spawn]->set_nature(tile_id);//0
        spawn_tiles[num_spawn]->set_base_sprite(&texture_nature_);
        sf::Vector2i sprite_type = sprite_nature_[overworld::sheet_nature::ROCK_1];
        spawn_tiles[num_spawn]->set_sprite_coordinates(&sprite_type,
                                    overworld::type_texture::BASE_IMAGE);
        // to help avoiding mointain spawn tiles beeing to placed adjacent to
        //    another mountain spawn, the surrounding tile to each mountain is
        //    replaced with another. And later cleaned up.
        if(require_border)
        {
            std::cout << "hello" << std::endl;
            for(unsigned i=0;i<spawn_tiles[num_spawn]->get_surrounding_tile_length();i++)
            {
                spawn_tiles[num_spawn]->get_surrounding_tile(i)->
                        set_base_sprite(&texture_nature_);
            }
            std::cout << "require border == true" << std::endl;
            change_surrounding_to(overworld::sheet_nature::ROCK_2,
                            spawn_tiles[num_spawn],
                            &sprite_nature_[overworld::sheet_nature::ROCK_2]);
        }
    }

    nature_tiles_.push_back(spawn_tiles);

    //cleanup to replace all tiles with nature id 10 to 3.
    unsigned tile = overworld::sheet_nature::ROCK_2;
    for(unsigned height = 0; height < hex_grid_.size(); height++)
    {
        for(unsigned width = 0; width < hex_grid_[0].size(); width++)
        {
            if(hex_grid_[height][width].get_nature() == tile)
            {
                std::cout << "removing evetything" << std::endl;
                hex_grid_[height][width].set_nature(overworld::
                                                        sheet_nature::ROCK_1);
            }
        }
    }
    // this must be moved to the structure department. and count for mountains.
    /*for(unsigned num_tiles = 0; num_tiles < num_aviable_tiles; num_tiles++)
    {
        unsigned type = overworld::texture_reference::RAVEL_2;
        change_nearby_tile_to_same(hex_grid_, nature_tiles_[0], type, overworld::nation::NEUTRAL_, false, max_bordering);
    }*/
    for(unsigned num_tiles = 0; num_tiles < num_aviable_tiles; num_tiles++)
    {
        change_nearby_tile_to_same(hex_grid_, nature_tiles_[0],
                                   overworld::sheet_nature::ROCK_1,
                                   overworld::sheet_fraction::NEUTRAL_PLANE,
                                   false, max_bordering);
    }
}
void hex_grid_creator::change_surrounding_to(const unsigned type, hex_sprite* hex, sf::Vector2i* tex)
{
    std::cout << type << std::endl;
    for(unsigned vector_length = 0; vector_length < hex->get_surrounding_tile_length(); vector_length ++)
    {
        //hex->get_surrounding_tile(vector_length)->set_nature(type);
        sf::Vector2i sprite_type = *tex;

        hex->get_surrounding_tile(vector_length)->
                    set_sprite_coordinates(&sprite_type,
                            overworld::type_texture::BASE_IMAGE);
    }
}
void hex_grid_creator::variate_background_tiles(std::vector<hex_hold_>& hex_grid_)
{
    unsigned counter = 0;
    unsigned type = overworld::nature::FORREST_;
    unsigned type_end;
    for(unsigned height=0; height < stats.grid_height_; height++)
    {
        for(unsigned width=0; width < stats.grid_width_;width++)
        {
            if(hex_grid_[height][width].get_nature() == type)
            {
                counter ++;std::cout << counter << std::endl;
                unsigned type_case = rand() % 4;
                std::cout << type_case << std::endl;
                type_end = (rand() % stats.finished_nature[type_case]);
                if(type_case == 0)
                {
                    type_end += overworld::sheet_nature::GRASS_1;
                }
                else if(type_case == 1)
                {
                    type_end += overworld::sheet_nature::HILL_1;
                }
                else if(type_case == 2)
                {
                    type_end += overworld::sheet_nature::LAKE_1;
                }
                else if(type_case == 3)
                {
                    type_end += overworld::sheet_nature::ROCK_1;
                }
                std::cout << " setting tile " << height << ", " << width << ". to: " << type_end << std::endl;
                sf::Vector2i sprite_type = sprite_nature_[type_end];
                hex_grid_[height][width].set_base_sprite(&texture_nature_);
                hex_grid_[height][width].set_sprite_coordinates(&sprite_type,
                                overworld::type_texture::BASE_IMAGE);
            }
        }
    }
}
void hex_grid_creator::generate_captial_cities(
                                    nation_group_& group_tiles,
                                    const unsigned tile_id,
                                    const unsigned group)
{
    unsigned scelected_tile;
    do
    {
        scelected_tile = rand() % group_tiles.size();
    }
    while(group_tiles[scelected_tile]->get_surrounding_tile_length() < 6);

    group_tiles[scelected_tile]->set_nature(tile_id);
    overworld::nation id_i;
    overworld::sheet_fraction city_id;
    sf::Vector2i city_1;
    sf::Vector2i city_2;
    sf::Vector2i city_3;
    switch(tile_id)
    {
        case 0: id_i = overworld::nation::RED_;
                city_1 = sprite_factions_[overworld::sheet_fraction::RED_CITY_1];
                city_2 = sprite_factions_[overworld::sheet_fraction::RED_CITY_2];
                city_3 = sprite_factions_[overworld::sheet_fraction::RED_CITY_3];     break;
        case 1: id_i = overworld::nation::GREEN_;
                city_1 = sprite_factions_[overworld::sheet_fraction::GREEN_CITY_1];
                city_2 = sprite_factions_[overworld::sheet_fraction::GREEN_CITY_2];
                city_3 = sprite_factions_[overworld::sheet_fraction::GREEN_CITY_3];  break;
        case 2: id_i = overworld::nation::BLUE_;
                city_1 = sprite_factions_[overworld::sheet_fraction::BLUE_CITY_1];
                city_2 = sprite_factions_[overworld::sheet_fraction::BLUE_CITY_2];
                city_3 = sprite_factions_[overworld::sheet_fraction::BLUECITY_3];    break;
        case 3: id_i = overworld::nation::SPAWN_;
                city_1 = sprite_factions_[overworld::sheet_fraction::SPAWN_POINT];   break;
    }
    group_tiles[scelected_tile]->set_nation(id_i);

    //change tile_
    group_tiles[scelected_tile]->set_base_sprite(&texture_factions_);
    group_tiles[scelected_tile]->set_sprite_coordinates(&city_1,
                                overworld::type_texture::BASE_IMAGE);
    if(tile_id < 3)
    {
        group_tiles[scelected_tile]->get_surrounding_tile(4)->set_base_sprite(&texture_factions_);
        group_tiles[scelected_tile]->get_surrounding_tile(5)->set_base_sprite(&texture_factions_);

        group_tiles[scelected_tile]->get_surrounding_tile(4)->set_sprite_coordinates(&city_2,
                                overworld::type_texture::BASE_IMAGE);
        group_tiles[scelected_tile]->get_surrounding_tile(5)->set_sprite_coordinates(&city_3,
                                overworld::type_texture::BASE_IMAGE);
    }
    const unsigned spawn = overworld::nation::SPAWN_;
    if(tile_id == spawn)
        spawn_point_ = group_tiles[scelected_tile];
}
hex_sprite* hex_grid_creator::get_spawn()
{
    return spawn_point_;
}
const overworld::state hex_grid_creator::pass_state(
                                        const std::vector<hex_hold_>& hex_grid_,
                                        const unsigned row,
                                        const unsigned col) const
{
    return(hex_grid_[col][row].get_occupation());
}
void hex_grid_creator::set_borders(std::vector<hex_hold_>& hex_grid_)
{
    for(unsigned col=0; col < hex_grid_.size(); col++)
    {
        for(unsigned row=0; row < hex_grid_[0].size();row++)
        {
            sf::Vector2i cords = sprite_border_[overworld::sheet_border::BLANK];
            hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::BORDER_0);
            hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::BORDER_1);
            hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::BORDER_2);
            hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::BORDER_3);
            hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::BORDER_4);
            hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::BORDER_5);

            for(unsigned border_tile =0;
                    border_tile < hex_grid_[col][row].get_surrounding_tile_length();
                    border_tile ++)
            {
                unsigned this_nation = hex_grid_[col][row].get_nation();
                unsigned other_nation= hex_grid_[col][row].get_surrounding_tile(border_tile)->get_nation();
                //std::cout << this_nation << " versus " << other_nation << std::endl;
                if(this_nation != other_nation && this_nation < 3)
                {
                    math::vector2f this_coord= hex_grid_[col][row].get_world_pos();
                    math::vector2f other_coord = hex_grid_[col][row].get_surrounding_tile(border_tile)->get_world_pos();

                    unsigned sprite_value =0;
                    switch(this_nation)
                    {
                    case 1:sprite_value+=  6;break;
                    case 2:sprite_value+= 12;break;
                    }
                    overworld::type_texture sprite_layer;

                    // compare coords
                    if(this_coord.x > other_coord.x && this_coord.y > other_coord.y)
                    {
                        sprite_layer = overworld::type_texture::BORDER_0;
                        sprite_value += 4;
                    }
                    else if(this_coord.x < other_coord.x && this_coord.y > other_coord.y)
                    {
                        sprite_layer = overworld::type_texture::BORDER_1;
                        sprite_value += 3;
                    }
                    else if(this_coord.x > other_coord.x && this_coord.y == other_coord.y)
                    {
                        sprite_layer = overworld::type_texture::BORDER_2;
                        sprite_value += 5;
                    }
                    else if(this_coord.x < other_coord.x && this_coord.y == other_coord.y)
                    {
                        sprite_layer = overworld::type_texture::BORDER_3;
                        sprite_value += 2;
                    }
                    else if(this_coord.x > other_coord.x && this_coord.y < other_coord.y)
                    {
                        sprite_layer = overworld::type_texture::BORDER_4;
                        sprite_value += 0;
                    }
                    else if(this_coord.x < other_coord.x && this_coord.y < other_coord.y)
                    {
                        sprite_layer = overworld::type_texture::BORDER_5;
                        sprite_value += 1;
                    }
                    //std::cout << "sprite_value" << sprite_value << ", at " << col << "_" << row <<std::endl;
                    sf::Vector2i cords = sprite_border_[sprite_value];
                    hex_grid_[col][row].set_sprite_coordinates(&cords, sprite_layer);
                    // place based on placement.
                }
            }

        }
    }
}

} // namespace overworld
} // namespace icarus
