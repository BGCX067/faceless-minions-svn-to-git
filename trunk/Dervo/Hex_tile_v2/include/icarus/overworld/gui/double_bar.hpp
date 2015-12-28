#ifndef ICARUS_OVERWORLD_GUI_DOUBLE_BAR_HPP_
#define ICARUS_OVERWORLD_GUI_DOUBLE_BAR_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/utils.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class double_bar : public sf::Drawable
{
protected:
    math::vector2f bar_size_;
    sf::RectangleShape background_;
    sf::RectangleShape main_bar_;
    sf::RectangleShape second_bar_;

    float main_initial_width_, second_initial_width_;
    float main_target_, secondary_target_;
    float main_timer_, secondary_timer_;
    float main_t_max_, secondary_t_max_;
    bool overflow_;
    bool main_full_;
    bool second_full_;
    bool gradient_enabled_;
    std::vector<uint32_t> colors_;

    void interpolate_color();

public:
    double_bar(const sf::Color& main_color,
               const sf::Color& secondary_color,
               const sf::Color& bg_color,
               const math::vector2f& bar_size);
    virtual ~double_bar();

    virtual void set_main_fill(float percentage, float time = 0.0f);
    virtual void set_secondary_fill(float percentage, float time = 0.0f);
    virtual void set_position(const math::vector2f& new_pos);
    math::vector2f get_position() const;
    virtual void update(double delta);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    virtual void enable_gradient(const std::vector<uint32_t>& colors);

    virtual bool get_overflow() const {return overflow_;}
    virtual void reset_overflow() { overflow_ = false; }
    virtual bool main_is_full() const { return main_full_; }
    virtual bool second_is_full() const { return second_full_; }
};
} // namespace gui
} // namespace overworld
} // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_DOUBLE_BAR_HPP_

