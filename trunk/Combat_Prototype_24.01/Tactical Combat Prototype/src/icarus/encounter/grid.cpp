#include <cmath>
#include <iostream>

#include "icarus/encounter/grid.hpp"

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
} // namespace encounter
} // namespace icarus
