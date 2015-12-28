#include <cmath>
#include <iostream>

#include "icarus/encounter/grid.hpp"
#include "icarus/encounter/character_base.hpp"

namespace icarus
{
namespace encounter
{
grid::grid() :
    GRID_X_(40.0f),
    GRID_Y_(250.0f),
    row_count_(0),
    col_count_(0),
    display_tiles_(false),
    tile_width_(50.0f),
    tile_height_(40.0f),
    grid_bounds_(0.0f, 0.0f, 0.0f, 0.0f),
    selected_tile_(NULL),
    previous_tile_(NULL)
{
}

grid::~grid()
{
    for (unsigned row = 0; row < grid_vector_.size(); ++row)
    {
        for (unsigned col = 0; col < grid_vector_[row].size(); ++col)
        {
            if (grid_vector_[row][col])
                delete grid_vector_[row][col];
        }
    }
}

bool grid::generate_grid(const unsigned rows,
                                   const unsigned cols,
                                   const float tile_radius)
{
    row_count_ = rows;
    col_count_ = cols;

    for (unsigned row = 0; row < row_count_; ++row)
    {
        grid_row temp_row;
        for (unsigned col = 0; col < col_count_; ++col)
        {
            temp_row.push_back(create_tile(row, col));
        }
        grid_vector_.push_back(temp_row);
    }
    grid_bounds_ = sf::FloatRect(GRID_X_, GRID_Y_,
                                 col_count_ * tile_width_,
                                 row_count_ * tile_height_);
    return true;
}

void grid::draw(sf::RenderTarget& target,
                          sf::RenderStates states) const
{
    for (unsigned row = 0; row < grid_vector_.size(); ++row)
    {
        for (unsigned col = 0; col < grid_vector_[row].size(); ++col)
        {
            // TODO:: make grid tiles drawable
            grid_vector_[row][col]->draw(target, states);
        }
    }
}

grid_tile* grid::create_tile(const unsigned row,
                            const unsigned col) const
{
    math::vector2f world_pos = math::vector2f(col * tile_width_ +
                                            (row % 2 ? (tile_width_*0.5f) : 0) + GRID_X_,
                                      row * (tile_height_*0.75f) + GRID_Y_);
    int adj_col = (row == 0 ? col : col - floor(row * 0.5));
    math::vector2i vec_pos = math::vector2i(adj_col, row);
    grid_tile* temp_tile = new grid_tile(vec_pos, world_pos);
    temp_tile->load_sprite();
    // TODO: un-hardcode
    if (col > 15)
        temp_tile->set_state(tile_state::out_of_bounds);

    return temp_tile;
}

grid_tile* grid::get_tile_at(unsigned row, unsigned col, bool untranslated) const
{
    if (!untranslated)
        col = (row == 0 ? col : col + floor(row * 0.5));
    if (row < grid_vector_.size() && col < grid_vector_[row].size())
        return grid_vector_[row][col];
    else
        return NULL;
}

void grid::add_surrounding_tiles(const math::vector2i& vec_pos,
                                 std::vector<path_node*>& node_vec,
                                 bool ai)
{
    node_vec.clear();
    for (int row_offset = -1; row_offset < 2; ++row_offset)
    {
        int limit = row_offset != 0 ? 1 : 2;
        for (int col_offset = -1; col_offset < limit; ++col_offset)
        {
            int adj_col = (row_offset == -1 ? col_offset + 1 : col_offset);
            const grid_tile* const tmp_tile = get_tile_at(
                                                        vec_pos.y + row_offset,
                                                        vec_pos.x + adj_col);
            if (!tmp_tile)
            {
                continue;
            }
            else if ((tmp_tile->get_state() == tile_state::unoccupied ||
                      (tmp_tile->get_state() == tile_state::out_of_bounds && ai))
                     && vec_pos != tmp_tile->get_vector_pos())
            {
                path_node* tmp_node = new path_node(tmp_tile->get_vector_pos());
                tmp_node->g = 10;
                node_vec.push_back(tmp_node);
            }
        }
    }
}

int grid::compute_h(const math::vector2i& start_pos,
                    const math::vector2i& end_pos) const
{
    int h1 = abs(end_pos.x - start_pos.x);
    int h2 = abs(end_pos.y - start_pos.y);
    return math::highest(math::highest(h1, h2), abs(h2 - h1)) * 10;
}

grid_tile* grid::get_tile_at(const math::vector2i& vec_pos, bool untranslated) const
{
    return get_tile_at(vec_pos.y, vec_pos.x, untranslated);
}

grid_tile* grid::get_tile_at(const math::vector2f& world_pos) const
{
    std::vector<grid_tile*> selected_tiles;
    for (unsigned row = 0; row < grid_vector_.size(); ++row)
    {
        for (unsigned col = 0; col < grid_vector_[row].size(); ++col)
        {
            if (grid_vector_[row][col]->get_bounds().
                contains(world_pos.get_sf_vector()))
            {
                selected_tiles.push_back(grid_vector_[row][col]);
            }
        }
    }
    if (selected_tiles.size() == 2)
    {
        sf::FloatRect intersection;
        if (selected_tiles[0]->get_bounds().intersects(selected_tiles[1]->
                                                       get_bounds(),
                                                       intersection))
        {
            int left = (selected_tiles[0]->get_world_pos().x <
                        selected_tiles[1]->get_world_pos().x ? 0 : 1);

            math::vector2f p1(intersection.left, intersection.top);
            math::vector2f p2(intersection.left,
                              intersection.top + intersection.height);
            math::vector2f p3(intersection.left + intersection.width,
                              intersection.top +
                              (selected_tiles[left]->get_world_pos().y >
                               selected_tiles[1-left]->get_world_pos().y ?
                               intersection.height : 0));

            int index = 0;
            if (math::point_in_triangle(p1, p2, p3, world_pos))
            {
                index = left;
            }
            else
            {
                index = 1 - left;
            }
            return selected_tiles[index];
        }
    }
    else if (selected_tiles.size() == 1)
    {
        return selected_tiles[0];
    }
    return NULL;
}

void grid::handle_mouse_over(math::vector2i mouse_xy)
{
    math::vector2f float_xy(static_cast<float>(mouse_xy.x),
                            static_cast<float>(mouse_xy.y));
    sf::FloatRect grid_rect(sf::Vector2f(GRID_X_ - (tile_width_*0.5f), GRID_Y_ - (tile_height_*0.5f)), sf::Vector2f(
                            static_cast<float>(col_count_) *
                            tile_width_ + (tile_width_*0.5f),
                            static_cast<float>(row_count_) *
                            (tile_height_*0.75f) + (tile_height_*0.375)));
    if (!grid_rect.contains(float_xy.get_sf_vector()))
    {
        previous_tile_ = selected_tile_;
        selected_tile_ = NULL;
        if (previous_tile_ != NULL)
            previous_tile_->set_hover_state(false);
        return;
    }

    grid_tile* selected = get_tile_at(float_xy);

    if (selected != NULL && selected_tile_ != selected)
    {
        previous_tile_ = selected_tile_;
        selected_tile_ = selected;
        selected_tile_->set_hover_state(true);
        if (previous_tile_ != NULL)
            previous_tile_->set_hover_state(false);
    }
}

void grid::toggle_tile_display(bool display)
{
    if (display == display_tiles_ && !display)
        return;
    display_tiles_ = display;
    for (unsigned row = 0; row < grid_vector_.size(); ++row)
    {
        for (unsigned col = 0; col < grid_vector_[row].size(); ++col)
        {
            grid_vector_[row][col]->set_hover_state(display);
        }
    }
}

bool grid::get_path_to_tile(const math::vector2i& target_tile,
                            character_base* active_char)
{
    if (active_char->get_target_vec_pos() == target_tile)
        return false;
    bool ai = active_char->is_ai();
    std::vector<path_node*> open_list, closed_list, surrounding;
    path_node* starting_node = new path_node(active_char->get_target_vec_pos());
    starting_node->g = 0;
    starting_node->h = compute_h(active_char->get_target_vec_pos(), target_tile);
    starting_node->update_f();
    open_list.push_back(starting_node);

    path_node* current_node = NULL;
    bool target_found = false;
    while (!target_found)
    {
        current_node = NULL;
        int lowest_f_index = -1;
        for (unsigned i = 0; i < open_list.size(); ++i)
        {
            if (!current_node || open_list[i]->f < current_node->f)
            {
                current_node = open_list[i];
                lowest_f_index = i;
            }
        }

        if (!current_node)
            break;
        open_list.erase(open_list.begin() + lowest_f_index);
        closed_list.push_back(current_node);

        if (current_node->vec_pos == target_tile)
        {
            target_found = true;
            break;
        }

        add_surrounding_tiles(current_node->vec_pos, surrounding, ai);
        for (int surr_i = 0; surr_i < static_cast<int>(surrounding.size());
             ++surr_i)
        {
            bool erased = false;
            for (unsigned clos_i = 0; clos_i < closed_list.size(); ++clos_i)
            {
                if (surrounding[surr_i]->vec_pos ==
                    closed_list[clos_i]->vec_pos)
                {
                    delete surrounding[surr_i];
                    surrounding.erase(surrounding.begin() + surr_i);
                    --surr_i;
                    erased = true;
                    break;
                }
            }
            if (erased)
                continue;
            for (unsigned open_i = 0; open_i < open_list.size(); ++open_i)
            {
                if (surrounding[surr_i]->vec_pos ==
                    open_list[open_i]->vec_pos)
                {
                    if (surrounding[surr_i]->g + current_node->g <
                        open_list[open_i]->g)
                    {
                        open_list[open_i]->parent = current_node;
                        open_list[open_i]->g = surrounding[surr_i]->g +
                                               current_node->g;
                        open_list[open_i]->update_f();
                    }

                    delete surrounding[surr_i];
                    surrounding.erase(surrounding.begin() + surr_i);
                    --surr_i;
                    erased = true;
                    break;
                }
            }
            if (erased)
                continue;

            surrounding[surr_i]->g += current_node->g;
            surrounding[surr_i]->h = compute_h(surrounding[surr_i]->vec_pos,
                                               target_tile);
            surrounding[surr_i]->update_f();
            surrounding[surr_i]->parent = current_node;
            open_list.push_back(surrounding[surr_i]);
            surrounding.erase(surrounding.begin() + surr_i);
            --surr_i;
        }
    }

    // TODO:: FIX MEMORY LEAKS!
    if (target_found)
    {
        std::vector<path_node*> new_path;
        path_node* node = current_node;
        while (node != NULL)
        {
            new_path.push_back(node);
            node = node->parent;
        }

        for (unsigned i = 0; i < open_list.size(); ++i)
            delete open_list[i];
        for (unsigned i = 0; i < closed_list.size(); ++i)
        {
            bool is_in_path = false;
            for (unsigned k = 0; k < new_path.size(); ++k)
            {
                if (closed_list[i] == new_path[k])
                    is_in_path = true;
            }
            if (!is_in_path)
                delete closed_list[i];
        }
        active_char->set_target_path(new_path);
        return true;
    }
    else
    {
        for (unsigned i = 0; i < open_list.size(); ++i)
            delete open_list[i];
        for (unsigned i = 0; i < closed_list.size(); ++i)
            delete closed_list[i];
        return false;
    }
}
void grid::get_tiles_in_radius(const math::vector2i& origin,
                               unsigned radius,
                               std::vector<grid_tile*>& results) const
{
    for (unsigned row = 0; row < grid_vector_.size(); ++row)
    {
        for (unsigned col = 0; col < grid_vector_[row].size(); ++col)
        {
            if (grid_vector_[row][col]->get_range_from(origin) <= radius)
                results.push_back(grid_vector_[row][col]);
        }
    }
}

grid_tile* grid::get_random_tile() const
{
    int row = rand()%grid_vector_.size();
    int col = rand()%grid_vector_[row].size();
    return grid_vector_[row][col];
}
} // namespace encounter
} // namespace icarus
