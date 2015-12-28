
#ifndef ICARUS_ENCOUNTER_GRID_HPP_
#define ICARUS_ENCOUNTER_GRID_HPP_

#include <vector>

#include <SFML/Graphics.hpp>

#include "grid_tile.hpp"
#include "icarus/encounter.hpp"

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
    bool display_tiles_;
    float tile_width_;
    float tile_height_;
    sf::FloatRect grid_bounds_;

    grid_tile* selected_tile_;
    grid_tile* previous_tile_;

    grid_tile* create_tile(const unsigned row, const unsigned col) const;

    void add_surrounding_tiles(const math::vector2i& vec_pos,
                               std::vector<path_node*>& node_vec);
    int compute_h(const math::vector2i& start_pos,
                  const math::vector2i& end_pos) const;
protected:

public:
    grid();
    ~grid();
    bool generate_grid(const unsigned rows, const unsigned cols,
                       const float tile_radius);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    grid_tile* get_tile_at(unsigned row, unsigned col) const;
    grid_tile* get_tile_at(const math::vector2i& vec_pos) const;
    grid_tile* get_tile_at(const math::vector2f& world_pos) const;
    void handle_mouse_over(math::vector2i mouse_xy);
    void toggle_tile_display(bool display);
    bool get_tile_display() const { return display_tiles_; }
    void get_path_to_tile(const math::vector2i& target_tile,
                          character_base* active_char);
    grid_tile* get_random_tile() const;

    grid_tile* get_selected_tile() const { return selected_tile_; }
    sf::FloatRect get_bounds() const { return grid_bounds_; }
    void get_tiles_in_radius(const math::vector2i& origin,
                             unsigned radius,
                             std::vector<grid_tile*>& results) const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_GRID_HPP_
