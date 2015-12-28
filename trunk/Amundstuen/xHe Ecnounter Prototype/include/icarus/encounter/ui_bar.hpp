#ifndef ICARUS_ENCOUNTER_UI_BAR_HPP_
#define ICARUS_ENCOUNTER_UI_BAR_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace encounter
{
namespace label_position
{
enum vertical
{
    v_top,
    v_bottom,
    v_center
};
enum horizontal
{
    h_left,
    h_right,
    h_center
};
} // namespace label_position
class ui_bar : public sf::Drawable
{
protected:
    sf::RectangleShape* background_;
    sf::RectangleShape* bar_;
    sf::Text* label_;
    label_position::vertical label_v_pos_;
    label_position::horizontal label_h_pos_;
    float bar_width_;
    float bar_height_;

public:
    ui_bar(const sf::Color& bar_color,
           const sf::Color& background_color,
           const std::string& label);
    virtual ~ui_bar();

    void set_label(const std::string& new_label);
    virtual void set_bar_fill(float percentage); // 0.0f - 1.0f

    void set_label_position(label_position::vertical v_pos,
                            label_position::horizontal h_pos);
    virtual void set_position(const math::vector2f& new_pos);
    math::vector2f get_position();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_UI_BAR_HPP_
