#ifndef ICARUS_UI_SLIDER_HPP_
#define ICARUS_UI_SLIDER_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/utils/yth_handler.hpp"
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace ui
{
class slider : public sf::Drawable
{
private:
    sf::Sprite bar_graphic_;
    sf::Sprite slider_graphic_;
    sf::Text value_label_;

    bool label_enabled_;

    unsigned min_value_;
    unsigned max_value_;

    bool drag_enabled_;

    unsigned drag_min_offset_;
    unsigned drag_max_offset_;

    unsigned current_value_;
    bool is_enabled_;

    void update_slider(const math::vector2f& pos);

public:
    slider();
    virtual ~slider();

    void setup(utils::yth_node* node, unsigned initial_value);
    void handle_input(const std::vector<sf::Event>& input_events);
    void set_value(unsigned value);
    void set_position(const math::vector2f& pos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    unsigned get_value() const { return current_value_; }
    void set_enabled(bool status) { is_enabled_ = status; }
};
}   // namespace ui
}   // namespace icarus

#endif // ICARUS_UI_SLIDER_HPP_
