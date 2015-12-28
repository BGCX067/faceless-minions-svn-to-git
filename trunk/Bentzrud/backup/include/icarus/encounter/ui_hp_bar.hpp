#ifndef ICARUS_ENCOUNTER_UI_HP_BAR_HPP_
#define ICARUS_ENCOUNTER_UI_HP_BAR_HPP_

#include "ui_bar.hpp"

namespace icarus
{
namespace encounter
{
class ui_hp_bar : public ui_bar
{
protected:
    sf::RectangleShape* damage_bar_;
    float reduction_speed_;
    float reduction_delay_;
    float current_delay_;

public:
    ui_hp_bar();
    virtual ~ui_hp_bar();

    void set_color(const sf::Color& main_color);
    void set_position(const math::vector2f& new_pos);
    void update_bar(float delta);
    virtual void set_bar_fill(float percentage); // 0.0f - 1.0f

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_UI_HP_BAR_HPP_
