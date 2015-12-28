
#ifndef ICARUS_ENCOUNTER_GRID_HPP_
#define ICARUS_ENCOUNTER_GRID_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "grid_tile.hpp"

namespace icarus
{
namespace encounter
{
class grid : public sf::Drawable
{
private:
    const float GRID_X_;
    const float GRID_Y_;

    typedef std::vector<grid_tile*> grid_row;
    std::vector<grid_row> grid_vector_;
    unsigned row_count_;
    unsigned col_count_;
    float tile_width_;
    float tile_height_;
    float tile_spacing_;
    bool display_tiles_;
    sf::FloatRect grid_bounds_;

    grid_tile* selected_tile_;
    grid_tile* previous_tile_;

    grid_tile* create_tile(const unsigned row, const unsigned col,
                          const float width, const float height) const;
protected:

public:
    grid();
    ~grid();
    bool generate_grid(const unsigned rows, const unsigned cols,
                       const float tile_width, const float tile_height);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    grid_tile* get_tile_at(unsigned row, unsigned col) const;
    grid_tile* get_tile_at(const math::vector2u& vec_pos) const;
    void handle_mouse_over(math::vector2i mouse_xy);
    void toggle_tile_display(bool display);
    grid_tile* get_selected_tile() const { return selected_tile_; }
    sf::FloatRect get_bounds() const { return grid_bounds_; }
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_GRID_HPP_
