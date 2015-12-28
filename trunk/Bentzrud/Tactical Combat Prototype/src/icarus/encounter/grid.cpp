#include <cmath>
#include <iostream>

#include "icarus/encounter/grid.hpp"
#include "icarus/encounter/character.hpp"

namespace icarus
{
namespace encounter
{
grid::grid() :
    GRID_X_(50.0f),
    GRID_Y_(300.0f),
    row_count_(0),
    col_count_(0),
    tile_width_(0.0f),
    tile_height_(0.0f),
    tile_spacing_(0.0f),
    display_tiles_(false),
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
                                   const float tile_width,
                                   const float tile_height)
{
    row_count_ = rows;
    col_count_ = cols;
    tile_height_ = tile_height;
    tile_width_ = tile_width;
    tile_spacing_ = 1.0f;

    for (unsigned row = 0; row < row_count_; ++row)
    {
        grid_row temp_row;
        for (unsigned col = 0; col < col_count_; ++col)
        {
            temp_row.push_back(create_tile(row, col,
                                           tile_width_,
                                           tile_height_));
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
            target.draw(*(grid_vector_[row][col]->get_shape()), states);
        }
    }
}

grid_tile* grid::create_tile(const unsigned row,
                            const unsigned col,
                            const float width,
                            const float height) const
{
    math::vector2f world_pos = math::vector2f(col * (tile_width_ + tile_spacing_)
                                       + GRID_X_,
                                      row * (tile_height_ + tile_spacing_)
                                       + GRID_Y_);
    math::vector2u vec_pos = math::vector2u(col, row);
    grid_tile* temp_tile = new grid_tile(vec_pos, world_pos);
    temp_tile->generate_shape(width, height, tile_spacing_);

    return temp_tile;
}

grid_tile* grid::get_tile_at(unsigned row, unsigned col) const
{
    if (row < grid_vector_.size() && col < grid_vector_[row].size())
        return grid_vector_[row][col];
    else
        return NULL;
}

void grid::add_surrounding_tiles(const math::vector2u& vec_pos,
                                 std::vector<path_node*>& node_vec)
{
    node_vec.clear();
    for (int row_offset = -1; row_offset < 2; ++row_offset)
    {
        for (int col_offset = -1; col_offset < 2; ++col_offset)
        {
            const grid_tile* const tmp_tile = get_tile_at(
                                                        vec_pos.y + row_offset,
                                                        vec_pos.x + col_offset);
            if (!tmp_tile)
            {
                continue;
            }
            else if (tmp_tile->get_state() == tile_state::unoccupied)
            {
                path_node* tmp_node = new path_node(tmp_tile->get_vector_pos());
                if (row_offset != 0 && col_offset != 0)
                    tmp_node->g = 14;
                else
                    tmp_node->g = 10;
                node_vec.push_back(tmp_node);
            }
        }
    }
}

int grid::compute_h(const math::vector2u& start_pos,
                    const math::vector2u& end_pos) const
{
    int h = abs(end_pos.x - start_pos.x) + abs(end_pos.y - start_pos.y);
    return h * 10;
}

grid_tile* grid::get_tile_at(const math::vector2u& vec_pos) const
{
    return grid_vector_[vec_pos.y][vec_pos.x];
}

void grid::handle_mouse_over(math::vector2i mouse_xy)
{
    math::vector2f float_xy(static_cast<float>(mouse_xy.x),
                            static_cast<float>(mouse_xy.y));
    sf::FloatRect grid_rect(sf::Vector2f(GRID_X_, GRID_Y_), sf::Vector2f(
                            static_cast<float>(col_count_) *
                            (tile_width_ + tile_spacing_),
                            static_cast<float>(row_count_) *
                            (tile_height_ + tile_spacing_)));
    if (!grid_rect.contains(float_xy.get_sf_vector()))
    {
        previous_tile_ = selected_tile_;
        selected_tile_ = NULL;
        if (previous_tile_ != NULL)
            previous_tile_->set_hover_state(false);
        return;
    }

    float_xy -= math::vector2f(GRID_X_, GRID_Y_);
    unsigned col = static_cast<unsigned>(floor(float_xy.x /
                                               (tile_width_ + tile_spacing_)));
    unsigned row = static_cast<unsigned>(floor(float_xy.y /
                                               (tile_height_ + tile_spacing_)));

    if (selected_tile_ != grid_vector_[row][col])
    {
        previous_tile_ = selected_tile_;
        selected_tile_ = grid_vector_[row][col];
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

void grid::get_path_to_tile(const math::vector2u& target_tile,
                            character* active_char)
{
    if (active_char->get_vector_pos() == target_tile)
        return;
    std::vector<path_node*> open_list, closed_list, surrounding;
    path_node* starting_node = new path_node(active_char->get_vector_pos());
    starting_node->g = 0;
    starting_node->h = compute_h(active_char->get_vector_pos(), target_tile);
    starting_node->update_f();
    open_list.push_back(starting_node);

    path_node* current_node = NULL;
    bool target_found = false;
    while (!target_found)
    {
        current_node = NULL;
        int lowest_f_index;
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

        add_surrounding_tiles(current_node->vec_pos, surrounding);
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
    }
    else
    {
        for (unsigned i = 0; i < open_list.size(); ++i)
            delete open_list[i];
        for (unsigned i = 0; i < closed_list.size(); ++i)
            delete closed_list[i];
        std::cout << "No available path found." << std::endl;
    }
}
} // namespace encounter
} // namespace icarus
