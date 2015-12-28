#include "icarus/overworld/hex_grid_creator.hpp"
namespace icarus
{
namespace overworld
{
hex_grid_creator::hex_grid_creator() : spawn_point_(NULL)
{
    for(int i = 0; i <= 6; i++)
    {
        for(int k = 1; k <= 6; k++)
        {
            sf::Vector2i cord1(k*stats.pixel_width_, i*stats.pixel_height_);
            sprite_border_.push_back(cord1);
        }
        for(int j = 1; j < 10; j++)
        {
            sf::Vector2i cord2(j*stats.pixel_width_, i*stats.pixel_height_);
            sprite_base_.push_back(cord2);
            sprite_overlay_.push_back(cord2);
        }
    }
    texture_base_.loadFromFile(stats.url_base_);
    texture_border_faction_.loadFromFile(stats.url_borders_);
    texture_overlay_.loadFromFile(stats.url_overlay_);
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
            hex_sprite hex_class(overworld::type_frac::TYPE_FRAC_NONE,
                    overworld::type_ground::TYPE_GROUND_EMPTY ,
                    overworld::type_overlay::TYPE_OVERLAY_NONE,
                    overworld::state::UNREACHABLE_);

            //set textures
            std::vector<sf::Texture*> temp;
            temp.push_back(&texture_base_);
            temp.push_back(&texture_border_faction_);
            temp.push_back(&texture_overlay_);
            hex_class.set_sprite_texture(temp);

            sf::Vector2i cord1 = sprite_base_[overworld::sheet_ground::sheet_ground_none];
            sf::Vector2i cord2 = sprite_border_[overworld::sheet_border::sheet_border_none];
            sf::Vector2i cord3 = sprite_overlay_[overworld::sheet_overlays::so_empty];

            hex_class.set_sprite_coordinates(&cord1, overworld::type_texture::BASE_IMAGE);
            hex_class.set_sprite_coordinates(&cord3, overworld::type_texture::OVERLAY_);
            for(unsigned i=1;i<13; i++)
                hex_class.set_sprite_coordinates(&cord2, static_cast<overworld::type_texture>(i));

            //set position
            sf::Vector2f cords;
            cords.y = col * (stats.pixel_height_ - stats.hex_offset_);//197;
            cords.x = row * stats.pixel_width_;//173;

            int j = col%2;
            if(j==1){ cords.x += (stats.pixel_width_ / 2); }

            hex_class.set_tile_coordinates(cords);

            vec2.push_back(hex_class);
        }
    hex_grid_.push_back(vec2);
    }
    asign_nearby_tiles(hex_grid_);
}
bool hex_grid_creator::is_inside_bounds(const unsigned row, const unsigned col)
{
    if(!(col < 0) && !(col > (stats.grid_height_ -1)) &&
       !(row < 0) && !(row > (stats.grid_width_ -1)))
    {
        return true;
    }
    return false;
}
void hex_grid_creator::add_surrounding_tiles_to(std::vector<hex_hold_>& hex_grid_,
                                        const unsigned *num_width,
                                        const unsigned *num_height)
{
    int x, y;
    int is_one_or_zero = (*num_height +1) %2;
    for(int i = 0; i < 6; i++)
    {
        switch(i)
        {
        case 0: y = *num_height -1;  x = *num_width - is_one_or_zero;     break;
        case 1: y = *num_height -1;  x = *num_width +1 - is_one_or_zero;  break;
        case 2: y = *num_height;     x = *num_width -1;                   break;
        case 3: y = *num_height;     x = *num_width +1;                   break;
        case 4: y = *num_height +1;  x = *num_width - is_one_or_zero;     break;
        case 5: y = *num_height +1;  x = *num_width + 1 - is_one_or_zero; break;
        }
        if(is_inside_bounds(x , y))
            hex_grid_[*num_height][*num_width].set_surrounding_tile(&hex_grid_[y][x]);
    }
}
void hex_grid_creator::asign_nearby_tiles(std::vector<hex_hold_>& hex_grid_)
{
    for(unsigned col = 0; col < stats.grid_height_; col++)
        for(unsigned row = 0; row < stats.grid_width_; row++)
            add_surrounding_tiles_to(hex_grid_, &row, &col);
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
                                    std::vector<tile_group_>& nation_tiles_,
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

    tile_group_ n_group;

    // chose starting positions based on the number of "num_frac"
    for(unsigned i = 0; i < num_frac; i++)
    {

        const unsigned choosen_tile = rand() % spawn_tiles.size();
        switch (i)
        {
            case 0: spawn_tiles[choosen_tile]->set_nation(RED_);    break;
            case 1: spawn_tiles[choosen_tile]->set_nation(GREEN_);  break;
            case 2: spawn_tiles[choosen_tile]->set_nation(BLUE_);   break;
            case 3: spawn_tiles[choosen_tile]->set_nation(SPAWN_);  break;
            default:break;
        }
        n_group.push_back(spawn_tiles[choosen_tile]);
        nation_tiles_.push_back(n_group);
        n_group.clear();
        spawn_tiles.erase (spawn_tiles.begin()+choosen_tile);
    }
    for(unsigned num_tiles = 0; num_tiles < num_tiles_per_nation -1; num_tiles++)
        for(unsigned per_frac = 0; per_frac < static_cast<unsigned>(num_frac); per_frac ++)
            change_nearby_tile_to_same(hex_grid_, nation_tiles_[per_frac], per_frac, overworld::type_frac::TYPE_FRAC_NONE,true);
}
void hex_grid_creator::change_nearby_tile_to_same(
                                    std::vector<hex_hold_>& hex_grid_,
                                    tile_group_& group,
                                    const unsigned type_tile,
                                    const unsigned base,
                                    const bool is_nation,
                                    const unsigned max_border)
{
    std::vector<hex_sprite*> usable_tiles;
    std::vector<hex_sprite*> surrounding_tiles;
    for(unsigned per_member =0; per_member < group.size(); per_member++)
    {
        std::vector<unsigned> temp_ints;
        for(unsigned i=0;i<max_border;i++)
            if(i < group[per_member]->get_surrounding_tile_length())
                temp_ints.push_back(i);
        unsigned max_size = group[per_member]->get_surrounding_tile_length();
        if(max_size > max_border)
            max_size = max_border;

        for(unsigned vector_length = 0; vector_length < max_size; vector_length ++)
        {

            unsigned temp_int =rand() % temp_ints.size();
            if((is_nation && group[per_member]-> get_surrounding_tile(vector_length)->get_nation() == static_cast<overworld::type_frac>(base)) ||
               (!is_nation && group[per_member]->get_surrounding_tile(vector_length)->get_nature() == static_cast<overworld::type_ground>(base)))
            {
                surrounding_tiles.push_back(group[per_member]->get_surrounding_tile(temp_ints[temp_int]));
                temp_ints.erase(temp_ints.begin() + temp_int);
            }
        }
        usable_tiles.insert( usable_tiles.end(), surrounding_tiles.begin(), surrounding_tiles.end() );
        if(surrounding_tiles.size() > 0)
            surrounding_tiles.clear();
    }
    if(usable_tiles.size() > 0)
    {
        unsigned scelected_tile =  rand() % usable_tiles.size();
        sf::Vector2i sprite_type;
        if(is_nation)
        {
            usable_tiles[scelected_tile]->set_nation(static_cast<overworld::type_frac>(type_tile));
        }
        else
        {
            usable_tiles[scelected_tile]->set_nature(static_cast<overworld::type_ground>(type_tile));
        }
        group.push_back(usable_tiles[scelected_tile]);
    }
}
void hex_grid_creator::generate_ground_ranges(std::vector<hex_hold_>& hex_grid_,
                                  std::vector<tile_group_>& nation_tiles_,
                                  const unsigned tile_id,
                                  unsigned number_of_spawn_areas,
                                  const double percentage_beeing_this,
                                  const unsigned max_bordering,
                                  const bool require_border)
{
    double total_tiles = stats.grid_height_ * stats.grid_width_;
    total_tiles *= percentage_beeing_this;
    const unsigned num_aviable_tiles = static_cast<int>(total_tiles);

    std::vector<hex_sprite*> spawn_tiles;

    if(num_aviable_tiles < number_of_spawn_areas)
        number_of_spawn_areas = num_aviable_tiles;

    //Adds ground spawn points.
    for(unsigned num_spawn =0; num_spawn < number_of_spawn_areas;num_spawn ++)
    {
        unsigned col, row;
        do
        {
            col = rand() % (stats.grid_height_ -1);
            row = rand() % (stats.grid_width_ -1);
        }
        while(hex_grid_[col][row].get_nature() != static_cast<signed>(tile_id) &&
              hex_grid_[col][row].get_nature() != overworld::type_ground::TYPE_GROUND_NO_USE &&
              hex_grid_[col][row].get_is_used());
        spawn_tiles.push_back(&hex_grid_[col][row]);

        spawn_tiles[num_spawn]->set_nature(static_cast<overworld::type_ground>(tile_id));

        if(require_border)
            set_surrounding_to(overworld::type_::T_GROUND_TILE_, &hex_grid_[col][row],
                               overworld::type_ground::TYPE_GROUND_NO_USE, 0,1);

    }
    nation_tiles_.push_back(spawn_tiles);

    for(unsigned height = 0; height < hex_grid_.size(); height++)
        for(unsigned width = 0; width < hex_grid_[0].size(); width++)
            if(hex_grid_[height][width].get_nature() == overworld::type_ground::TYPE_GROUND_NO_USE)
                hex_grid_[height][width].set_nature(overworld::type_ground::TYPE_GROUND_EMPTY);

    for(unsigned num_tiles = 0; num_tiles < num_aviable_tiles; num_tiles++)
    {
        change_nearby_tile_to_same(hex_grid_,
                                   nation_tiles_[nation_tiles_.size()-1],
                                   tile_id,
                                   overworld::type_ground::TYPE_GROUND_EMPTY,
                                   false,
                                   max_bordering);
    }
    std::cout << tile_id << std::endl;
    //dubble for loop
    for(unsigned height = 0; height < hex_grid_.size(); height++)
    {
        for(unsigned width = 0; width < hex_grid_[0].size(); width++)
        {
            if(hex_grid_[height][width].get_nature() == static_cast<signed>(tile_id))
            {
                overworld::sheet_ground temp;
                switch(tile_id)
                {
                case FORREST_FLOOR_: temp = overworld::sheet_ground::WODS_1; break;
                case ROCK_:          temp = overworld::sheet_ground::ROCK_1; break;
                case WATTER_:        temp = overworld::sheet_ground::WATER_1; break;
                default: break;
                }
                sf::Vector2i sprite_type = sprite_base_[temp +(rand() % 4)];
                hex_grid_[height][width].set_sprite_coordinates(&sprite_type, overworld::type_texture::BASE_IMAGE);
            }
        }
    }
}

void hex_grid_creator::generate_captial_cities(tile_group_& group_tiles, const overworld::type_frac frac_id)
{
    sf::Vector2i city_1, city_2, city_3;
    unsigned scelected_tile;

    group_tiles[scelected_tile]->set_is_used(true);
    do{ scelected_tile = rand() % group_tiles.size(); }
    while(group_tiles[scelected_tile]->get_surrounding_tile_length() < 6);

    set_surrounding_to(overworld::type_::T_FRACTION_, group_tiles[scelected_tile],frac_id, 0,0);
    set_surrounding_to(overworld::type_::T_OVERLAY_, group_tiles[scelected_tile], overworld::type_overlay::TYPE_OVERLAY_NO_USE, 0,0);
    switch(frac_id)
    {
        case 0: city_1 = sprite_overlay_[overworld::sheet_overlays::CAP_RED_1];
                city_2 = sprite_overlay_[overworld::sheet_overlays::CAP_RED_2];
                city_3 = sprite_overlay_[overworld::sheet_overlays::CAP_RED_3];     break;
        case 1: city_1 = sprite_overlay_[overworld::sheet_overlays::CAP_GREEN_1];
                city_2 = sprite_overlay_[overworld::sheet_overlays::CAP_GREEN_2];
                city_3 = sprite_overlay_[overworld::sheet_overlays::CAP_GREEN_3];  break;
        case 2: city_1 = sprite_overlay_[overworld::sheet_overlays::CAP_BLUE_1];
                city_2 = sprite_overlay_[overworld::sheet_overlays::CAP_BLUE_2];
                city_3 = sprite_overlay_[overworld::sheet_overlays::CAP_BLUE_3];    break;
        case 3: city_1 = sprite_overlay_[overworld::sheet_overlays::SPAWN_POINT];   break;
        default:break;
    }

    //change tile_
    group_tiles[scelected_tile]->set_sprite_coordinates(&city_1, overworld::type_texture::OVERLAY_);
    group_tiles[scelected_tile]->set_is_used(true);
    set_surrounding_to(overworld::type_::T_OVERLAY_,group_tiles[scelected_tile],overworld::type_overlay::TYPE_OVERLAY_NO_USE,0, 0);

    if(frac_id < 3)
    {
        set_surrounding_to(overworld::type_::T_OVERLAY_,group_tiles[scelected_tile]->get_surrounding_tile(4),
                           overworld::type_overlay::TYPE_OVERLAY_NO_USE, 0, 0);
        set_surrounding_to(overworld::type_::T_OVERLAY_,group_tiles[scelected_tile]->get_surrounding_tile(5),
                           overworld::type_overlay::TYPE_OVERLAY_NO_USE,0, 0);
        group_tiles[scelected_tile]->get_surrounding_tile(4)->set_sprite_coordinates(&city_2, overworld::type_texture::OVERLAY_);
        group_tiles[scelected_tile]->get_surrounding_tile(4)->set_is_used(true);
        group_tiles[scelected_tile]->get_surrounding_tile(4)->set_overlay(overworld::type_overlay::CAPITAL_);
        group_tiles[scelected_tile]->get_surrounding_tile(5)->set_sprite_coordinates(&city_3, overworld::type_texture::OVERLAY_);
        group_tiles[scelected_tile]->get_surrounding_tile(5)->set_is_used(true);
        group_tiles[scelected_tile]->get_surrounding_tile(5)->set_overlay(overworld::type_overlay::CAPITAL_);
    }
    if(frac_id == overworld::type_frac::SPAWN_)
        spawn_point_ = group_tiles[scelected_tile];

    group_tiles[scelected_tile]->set_overlay(overworld::type_overlay::CAPITAL_);
}
void hex_grid_creator::set_borders(std::vector<hex_hold_>& hex_grid_)
{
    for(unsigned col=0; col < hex_grid_.size(); col++)
    {
        for(unsigned row=0; row < hex_grid_[0].size();row++)
        {
            for(unsigned border_tile =0;border_tile < hex_grid_[col][row].get_surrounding_tile_length();border_tile ++)
            {
                //regional_borders.
                bool do_nation = false;
                bool do_nature = false;
                unsigned this_nation = hex_grid_[col][row].get_nation();
                unsigned other_nation= hex_grid_[col][row].get_surrounding_tile(border_tile)->get_nation();
                unsigned this_nature = hex_grid_[col][row].get_nature();
                unsigned other_nature= hex_grid_[col][row].get_surrounding_tile(border_tile)->get_nature();

                if(this_nation != other_nation && this_nation < 3)
                    do_nation = true;
                if(this_nature != other_nature && this_nature != overworld::type_ground::PLANES_)
                    do_nature = true;

                math::vector2f this_coord= hex_grid_[col][row].get_world_pos();
                math::vector2f other_coord = hex_grid_[col][row].get_surrounding_tile(border_tile)->get_world_pos();

                overworld::type_texture nation_layer;
                overworld::type_texture nature_layer;
                unsigned                nation_value = 6*this_nation;
                unsigned                nature_value = overworld::sheet_border::N_ROCK_1 -6 + (6*this_nature);

                // compare coords
                if(this_coord.x > other_coord.x && this_coord.y > other_coord.y)
                {
                    if(do_nation)
                    {
                        nation_layer = overworld::type_texture::F_BORDER_0;
                        nation_value += 4;
                    }
                    if(do_nature)
                    {
                        nature_layer = overworld::type_texture::N_BORDER_0;
                        nature_value += 4;
                    }
                }
                else if(this_coord.x < other_coord.x && this_coord.y > other_coord.y)
                {
                    if(do_nation)
                    {
                        nation_layer = overworld::type_texture::F_BORDER_1;
                        nation_value += 3;
                    }
                    if(do_nature)
                    {
                        nature_layer = overworld::type_texture::N_BORDER_1;
                        nature_value += 3;
                    }
                }
                else if(this_coord.x > other_coord.x && this_coord.y == other_coord.y)
                {
                    if(do_nation)
                    {
                        nation_layer = overworld::type_texture::F_BORDER_2;
                        nation_value += 5;
                    }
                    if(do_nature)
                    {
                        nature_layer = overworld::type_texture::N_BORDER_2;
                        nature_value += 5;
                    }
                }
                else if(this_coord.x < other_coord.x && this_coord.y == other_coord.y)
                {
                    if(do_nation)
                    {
                        nation_layer = overworld::type_texture::F_BORDER_3;
                        nation_value += 2;
                    }
                    if(do_nature)
                    {
                        nature_layer = overworld::type_texture::N_BORDER_3;
                        nature_value += 2;
                    }
                }
                else if(this_coord.x > other_coord.x && this_coord.y < other_coord.y)
                {
                    if(do_nation)
                    {
                        nation_layer = overworld::type_texture::F_BORDER_4;
                        nation_value += 0;
                    }
                    if(do_nature)
                    {
                        nature_layer = overworld::type_texture::N_BORDER_4;
                        nature_value += 0;
                    }
                }
                else if(this_coord.x < other_coord.x && this_coord.y < other_coord.y)
                {
                    if(do_nation)
                    {
                        nation_layer = overworld::type_texture::F_BORDER_5;
                        nation_value += 1;
                    }
                    if(do_nature)
                    {
                        nature_layer = overworld::type_texture::N_BORDER_5;
                        nature_value += 1;
                    }
                }
                if(do_nation)
                {
                    sf::Vector2i cords = sprite_border_[nation_value];
                    hex_grid_[col][row].set_sprite_coordinates(&cords, nation_layer);
                }
                if(do_nature)
                {
                    sf::Vector2i cords = sprite_border_[nature_value];
                    hex_grid_[col][row].set_sprite_coordinates(&cords, nature_layer);
                }
            }
        }
    }
}
void hex_grid_creator::setup_overlay(std::vector<hex_hold_>& hex_grid_, std::vector<unsigned> num_each)
{
    for(unsigned col=0;col<hex_grid_.size();col++)
    {
        for(unsigned row=0;row<hex_grid_[0].size();row++)
        {
            if(hex_grid_[col][row].get_nature() == overworld::type_ground::ROCK_)
            {
                sf::Vector2i cord = sprite_base_[overworld::sheet_overlays::MOUNTAIN_1 + rand()%3];
                hex_grid_[col][row].set_sprite_coordinates(&cord, overworld::type_texture::OVERLAY_);
            }
            else if(hex_grid_[col][row].get_nature() == overworld::type_ground::FORREST_FLOOR_)
            {
                sf::Vector2i cord = sprite_base_[overworld::sheet_overlays::FORREST_1 + rand()%3];
                hex_grid_[col][row].set_sprite_coordinates(&cord, overworld::type_texture::OVERLAY_);
            }
        }
    }
    for(unsigned i=0;i<num_each.size();i++)
    {
        for(unsigned j=0;j<num_each[i];j++)
        {
            std::vector<hex_sprite*> temp_list = return_list(hex_grid_);
            unsigned temp_int;
            do
            {
                temp_int = rand() % temp_list.size();
            }
            while (temp_list[temp_int]->get_nature() != overworld::type_ground::PLANES_ &&
                   temp_list[temp_int]->get_overlay() == overworld::type_overlay::TYPE_OVERLAY_NONE &&
                   temp_list[temp_int]->get_overlay() == overworld::type_overlay::TYPE_OVERLAY_NO_USE &&
                   temp_list[temp_int]->get_is_used() == true);
            temp_list[temp_int]->set_overlay(static_cast<overworld::type_overlay>(i));
            switch(i)
            {
            case overworld::type_overlay::CITY_:
                set_surrounding_to(overworld::type_::T_OVERLAY_, temp_list[temp_int], overworld::type_overlay::TYPE_OVERLAY_NO_USE, 0,0);
                break;
            case overworld::type_overlay::CASTLE_:
                set_surrounding_to(overworld::type_::T_OVERLAY_, temp_list[temp_int], overworld::type_overlay::TYPE_OVERLAY_NO_USE, 0,0);
                break;
            case overworld::type_overlay::FORTRESS_:
                set_surrounding_to(overworld::type_::T_OVERLAY_, temp_list[temp_int], overworld::type_overlay::TYPE_OVERLAY_NO_USE, 0,0);
                break;
            default:
                break;
            }
        }
    }
    for(unsigned col=0; col < hex_grid_.size(); col++)
    {
        for(unsigned row=0; row < hex_grid_[0].size();row++)
        {
            if(hex_grid_[col][row].get_nature() == overworld::type_ground::PLANES_ &&
               hex_grid_[col][row].get_overlay() != overworld::type_overlay::CAPITAL_ &&
               hex_grid_[col][row].get_overlay() != overworld::type_overlay::TYPE_OVERLAY_NONE &&
               hex_grid_[col][row].get_overlay() != overworld::type_overlay::TYPE_OVERLAY_NO_USE)
            {
                std::cout << "hello!"  << std::endl;
                unsigned temp = hex_grid_[col][row].get_overlay()*3;
                temp += rand() % stats.finished_overlay[hex_grid_[col][row].get_overlay()];
                sf::Vector2i cords = sprite_overlay_[temp];
                hex_grid_[col][row].set_sprite_coordinates(&cords, overworld::type_texture::OVERLAY_);
            }
        }
    }
}
void hex_grid_creator::set_surrounding_to(
                            overworld::type_ id,
                            hex_sprite* target_tile,
                            unsigned type,
                            unsigned this_size,
                            unsigned max_size)
{
    for(unsigned num_tiles =0; num_tiles < target_tile->get_surrounding_tile_length(); num_tiles++)
    {
        if(id == T_OVERLAY_)
        switch(id)
        {
        case T_FRACTION_:   target_tile->get_surrounding_tile(num_tiles)->set_nation(static_cast<overworld::type_frac>(type)); break;
        case T_GROUND_TILE_:target_tile->get_surrounding_tile(num_tiles)->set_nature(static_cast<overworld::type_ground>(type)); break;
        case T_OVERLAY_:target_tile->get_surrounding_tile(num_tiles)->set_overlay(static_cast<overworld::type_overlay>(type)); break;
        default:break;
        }
        if(this_size < max_size)
            set_surrounding_to(id, target_tile->get_surrounding_tile(num_tiles),
                        type, this_size+1, max_size);
    }
}
std::vector<hex_sprite*> hex_grid_creator::return_list(std::vector<hex_hold_>& hex_grid_)
{
    std::vector<hex_sprite*> temp_list;
    for(unsigned col=0; col < hex_grid_.size(); col++)
        for(unsigned row=0; row < hex_grid_[0].size();row++)
            if(hex_grid_[col][row].get_overlay() == overworld::type_overlay::TYPE_OVERLAY_NONE)
                temp_list.push_back(&hex_grid_[col][row]);
    return temp_list;
}
void hex_grid_creator::variate_background_tiles(std::vector<hex_hold_>& hex_grid_)
{
    for(unsigned col=0;col<hex_grid_.size();col++)
        for(unsigned row=0;row<hex_grid_[0].size();row++)
            if(hex_grid_[col][row].get_nature() == overworld::type_ground::TYPE_GROUND_EMPTY)
            {
                sf::Vector2i sprite_type;
                if(rand() % 5 < 4)
                {
                    hex_grid_[col][row].set_nature(overworld::type_ground::PLANES_);
                    sprite_type = sprite_base_[overworld::sheet_ground::GRASS_1 + (rand() % 4)];
                }
                else
                {
                    hex_grid_[col][row].set_nature(overworld::type_ground::WATTER_);
                    sprite_type = sprite_base_[overworld::sheet_ground::WATER_1 + (rand() % 4)];
                }
                hex_grid_[col][row].set_sprite_coordinates(&sprite_type, overworld::type_texture::BASE_IMAGE);


            }

}
} // namespace overworld
} // namespace icarus
