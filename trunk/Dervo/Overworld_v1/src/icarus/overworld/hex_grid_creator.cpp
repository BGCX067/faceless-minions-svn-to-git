#include "icarus/overworld/hex_grid_creator.hpp"
namespace icarus
{
namespace overworld
{
hex_grid_creator::hex_grid_creator() : spawn_point_(NULL)
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            sf::Vector2i cord(j*stats.pixel_height_, i*stats.pixel_width_);
            sprite_holder_.push_back(cord);
        }
    }
    grid_texture_file_.loadFromFile(stats.url_hex_tiles_);
}

hex_grid_creator::~hex_grid_creator()
{
    //dtor
}
void hex_grid_creator::create_hex_grid()
{
    //asign sprite texture and coordiantes
    for(unsigned col = 0; col < stats.grid_height_; col++)
    {
        std::vector<hex_sprite> vec2;
        for(unsigned row = 0; row < stats.grid_width_; row++)
        {
            hex_sprite hex_class;
            hex_class.set_nation(3);
            hex_class.set_nature(3);
            hex_class.set_sprite_texture(&grid_texture_file_);
            sf::Vector2i sprite_type = sprite_holder_[hex_class.get_nation()];
            hex_class.set_sprite_coordinates(&sprite_type);

            sf::Vector2f cords;
            //set position
            cords.y = col * (stats.pixel_height_ - stats.hex_offset_);//197;
            cords.x = row * stats.pixel_width_;//173;

            int j = col;
            while(j > 1) { j -=2 ; }
            if(j==1){ cords.x += (stats.pixel_width_ / 2); }

            hex_class.set_tile_coordinates(cords);

            vec2.push_back(hex_class);
        }
    hex_grid_.push_back(vec2);
    }
}
bool hex_grid_creator::is_inside_bounds(const unsigned row, const unsigned col)
{
    if(!(col < 0) && !(col > (hex_grid_.size() -1)) &&
       !(row < 0) && !(row > (hex_grid_[0].size() -1)))

    {
        return true;
    }
    else
    {
        return false;
    }

}
void hex_grid_creator::change_variables(const unsigned *num_width,
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
void hex_grid_creator::asign_nearby_tiles()
{
    for(unsigned col = 0; col < stats.grid_height_; col++)
    {
        for(unsigned row = 0; row < stats.grid_width_; row++)
        {
            change_variables(&row, &col);
        }
    }
}
hex_sprite* hex_grid_creator::return_sprite(const unsigned row,
                                            const unsigned col)
{
    return &hex_grid_[col][row];
}
const hex_sprite* hex_grid_creator::return_const_sprite(const unsigned row,
                                            const unsigned col) const
{
    return &hex_grid_[col][row];
}
const overworld::state hex_grid_creator::pass_state(const unsigned row,
                                                    const unsigned col) const
{
    return hex_grid_[col][row].get_occupation();
}
const sf::FloatRect hex_grid_creator::pass_bounds(const unsigned row,
                                                  const unsigned col)
{
    sf::FloatRect temp = hex_grid_[col][row].return_bounds();
    temp.top += stats.hex_sprite_margin;
    temp.height -= stats.hex_sprite_margin;
    return temp;
}
void hex_grid_creator::generate_starter_fractions(const unsigned num_frac,
                                    const double percentage_uncaptured)
{
    double total_tiles = hex_grid_.size() * hex_grid_[0].size();
    total_tiles *= 1 - percentage_uncaptured;
    const unsigned num_aviable_tiles = static_cast<int>(total_tiles);
    const unsigned num_tiles_per_nation = num_aviable_tiles / num_frac -1;

    // Generate the 8 startingpositions.
    const unsigned col = hex_grid_.size() -1;
    const unsigned row = hex_grid_[0].size() -1;
    unsigned half_col = col/2;
    unsigned half_row = row/2;

    int i = col % 2;
    if(i == 1){ half_col ++;}
    else{       half_col += rand() % 1;}

    i = (row +1 ) % 2;
    if(i == 1){ half_row ++;}
    else{       half_row += rand() % 1;}

    nation_group_ n_group;

    std::vector<hex_sprite*> spawn_tiles;
    spawn_tiles.push_back(&hex_grid_[0][0]);
    spawn_tiles.push_back(&hex_grid_[0][row]);
    spawn_tiles.push_back(&hex_grid_[col][0]);
    spawn_tiles.push_back(&hex_grid_[col][row]);
    spawn_tiles.push_back(&hex_grid_[0][half_row]);
    spawn_tiles.push_back(&hex_grid_[col][half_row]);
    spawn_tiles.push_back(&hex_grid_[half_col][0]);
    spawn_tiles.push_back(&hex_grid_[half_col][row]);


    // chose starting positions based on the number of "num_frac"
    for(unsigned i = 0; i < num_frac; i++)
    {
        const unsigned choosen_tile = rand() % spawn_tiles.size();
        spawn_tiles[choosen_tile]->set_nation(6 + i);
        spawn_tiles[choosen_tile]->set_sprite_coordinates(
                    &sprite_holder_[spawn_tiles[choosen_tile]->get_nation()]);

        n_group.push_back(spawn_tiles[choosen_tile]);
        nation_tiles_.push_back(n_group);
        n_group.clear();
        spawn_tiles.erase (spawn_tiles.begin()+choosen_tile);
    }
    for(unsigned num_tiles = 0; num_tiles < num_tiles_per_nation; num_tiles++)
    {
        for(unsigned per_frac = 0; per_frac < num_frac; per_frac ++)
        {
           change_nearby_tile_to_same(nation_tiles_[per_frac], 6 + per_frac, 3, true);
        }
    }
}
void hex_grid_creator::set_tile_to(const hex_sprite* active_tile,
                                   const unsigned id)
{

}
void hex_grid_creator::change_nearby_tile_to_same(nation_group_& group,
                                                  const unsigned type_tile,
                                                  const unsigned base,
                                                  const bool is_nation,
                                                  const unsigned max_border)
{

    // fix max border


    std::vector<hex_sprite*> usable_tiles;
    for(unsigned per_member =0; per_member < group.size(); per_member++)
    {
        std::vector<hex_sprite*> surrounding_tiles;
        for(unsigned vector_length = 0; vector_length <
        group[per_member]->get_surrounding_tile_length(); vector_length ++)
        {
            if(surrounding_tiles.size() < max_border)
            {
                if(is_nation && group[per_member]->
                                    get_surrounding_tile(vector_length)->
                                    get_nation() == base)
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
            nation_tiles_[type_tile-6].push_back(usable_tiles[scelected_tile]);
            sprite_type = sprite_holder_[usable_tiles[scelected_tile]->
                        get_nation()];
            usable_tiles[scelected_tile]->set_sprite_coordinates(&sprite_type);
        }
        else
        {
            usable_tiles[scelected_tile]->set_nature(type_tile);
            group.push_back(usable_tiles[scelected_tile]);
            sprite_type = sprite_holder_[usable_tiles[scelected_tile]->
                        get_nature()];
            usable_tiles[scelected_tile]->set_sprite_coordinates(&sprite_type);
        }
    }
}

void hex_grid_creator::generate_nature_tiles(const unsigned tile_id,
                                    unsigned number_of_spawn_tiles,
                                    const double percentage_beeing_this,
                                    const unsigned max_bordering,
                                    const bool require_border)
{
    //
    double total_tiles = hex_grid_.size() * hex_grid_[0].size();
    total_tiles *= percentage_beeing_this;
    const unsigned num_aviable_tiles = static_cast<int>(total_tiles);

    std::vector<hex_sprite*> spawn_tiles;

    if(num_aviable_tiles < number_of_spawn_tiles)
        number_of_spawn_tiles = num_aviable_tiles;

    for(unsigned num_spawn =0; num_spawn < number_of_spawn_tiles;num_spawn ++)
    {
        unsigned col;
        unsigned row;
        do
        {
            col = rand() % (hex_grid_.size() -1);
            row = rand() % (hex_grid_[0].size() -1);
        }
        while(hex_grid_[col][row].get_nature() != 3);

        spawn_tiles.push_back(&hex_grid_[col][row]);

        spawn_tiles[num_spawn]->set_nature(tile_id);
        sf::Vector2i sprite_type = sprite_holder_[spawn_tiles[num_spawn]->
                        get_nature()];
        spawn_tiles[num_spawn]->set_sprite_coordinates(&sprite_type);
        if(require_border)
            change_surrounding_to(10, spawn_tiles[num_spawn]);
    }
    nature_tiles_.push_back(spawn_tiles);

    for(unsigned height = 0; height < hex_grid_.size(); height++)
    {
        for(unsigned width = 0; width < hex_grid_.size(); width++)
        {
            if(hex_grid_[height][width].get_nature() == 10)
            {
                hex_grid_[height][width].set_nature(3);
            }
        }
    }

    for(unsigned num_tiles = 0; num_tiles < num_aviable_tiles; num_tiles++)
    {
        unsigned type;
            switch (rand() % 3)
            {
            case 0: type =  1; break;
            case 1: type = 13; break;
            case 2: type = 14; break;
            }
        change_nearby_tile_to_same(nature_tiles_[nature_tiles_.size()-1], type, 3, false, max_bordering);
    }
}
void hex_grid_creator::change_surrounding_to(const unsigned type, hex_sprite* hex)
{
    for(unsigned vector_length = 0; vector_length < hex->get_surrounding_tile_length(); vector_length ++)
    {
        hex->get_surrounding_tile(vector_length)->set_nature(type);
        sf::Vector2i sprite_type = sprite_holder_[type];

        hex->get_surrounding_tile(vector_length)->
                    set_sprite_coordinates(&sprite_type);
    }
}
void hex_grid_creator::variate_background_tiles()
{
    for(unsigned height=0; height < hex_grid_.size(); height++)
    {
        for(unsigned width=0; width < hex_grid_[0].size();width++)
        {
            unsigned type;
            switch (rand() % 4)
            {
            case 0: type =  1; break;
            case 1: type = 11; break;
            case 2: type = 12; break;
            case 3: type = 3; break;
            }
            if(hex_grid_[height][width].get_nature() == 3)
            {
                hex_grid_[height][width].set_nature(type);
                sf::Vector2i sprite_type = sprite_holder_[type];

                hex_grid_[height][width].
                            set_sprite_coordinates(&sprite_type);
            }
        }
    }
}
void hex_grid_creator::generate_captial_cities( const unsigned tile_id,
                                                const unsigned group)
{
    //hello this is dog.
    const unsigned scelected_tile = rand() % nation_tiles_[group].size();
    nation_tiles_[group][scelected_tile]->set_nature(tile_id);
    if(tile_id == 9)
        spawn_point_ = nation_tiles_[group][scelected_tile];
    sf::Vector2i sprite_type =
                        sprite_holder_[nation_tiles_[group][scelected_tile]->
                        get_nature()];
    nation_tiles_[group][scelected_tile]->set_sprite_coordinates(&sprite_type);
}
hex_sprite* hex_grid_creator::get_spawn()
{
    return spawn_point_;
}
/*
hex_sprite* hex_grid_creator::hit_tile(math::vector2f)
{
    std::vector<hex_sprite*> =
    for(unsigned col=0; col < hex_grid_.size(); col++)
    {
        for(unsigned row=0; row < hex_grid_[0].size();row++)
        {

        }
    }

    if()
    else
    {
        return NULL;
    }
}
*/

} // namespace overworld
} // namespace icarus
