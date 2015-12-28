#ifndef ICARUS_ENCOUNTER_UI_HP_BAR_HPP_
#define ICARUS_ENCOUNTER_UI_HP_BAR_HPP_

#include "icarus/encounter/ui/bar.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
class hp_bar : public bar
{
protected:
    sf::RectangleShape* damage_bar_;
    float reduction_speed_;
    float reduction_delay_;
    float current_delay_;

public:
    hp_bar();
    virtual ~hp_bar();

    void set_color(const sf::Color& main_color);
    void set_position(const math::vector2f& new_pos);
    void update_bar(float delta);
    virtual void set_bar_fill(float percentage); // 0.0f - 1.0f
    void set_fill_instant(float percentage);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
} // namespace ui
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_UI_HP_BAR_HPP_
