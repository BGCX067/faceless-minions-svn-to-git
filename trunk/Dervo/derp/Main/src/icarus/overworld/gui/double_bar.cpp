#include "icarus/overworld/gui/double_bar.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
double_bar::double_bar(const sf::Color& main_color,
           const sf::Color& secondary_color,
           const sf::Color& bg_color,
           const math::vector2f& bar_size)
:
    bar_size_(bar_size),
    background_(bar_size+2.0f),
    main_bar_(bar_size_),
    second_bar_(bar_size_),
    main_initial_width_(0.0f),
    second_initial_width_(0.0f),
    main_target_(0.0f),
    secondary_target_(0.0f),
    main_timer_(0.0f),
    secondary_timer_(0.0f),
    main_t_max_(0.0f),
    secondary_t_max_(0.0f),
    overflow_(false),
    main_full_(false),
    second_full_(false),
    gradient_enabled_(false)
{
    background_.setFillColor(bg_color);
    main_bar_.setFillColor(main_color);
    second_bar_.setFillColor(secondary_color);
}
double_bar::~double_bar()
{
}

void double_bar::set_main_fill(float percentage, float time)
{
    if (percentage > 1.0f)
        overflow_ = true;
    main_target_ = math::clamp(percentage, 0.0f, 1.0f);
    if (time == 0.0f)
    {
        main_bar_.setSize(math::vector2f(bar_size_.x*percentage, bar_size_.y));
        if (gradient_enabled_)
            interpolate_color();
    }
    else
    {
        main_timer_ = 0.0f;
        main_t_max_ = time;
    }
    main_initial_width_ = main_bar_.getSize().x;
}
void double_bar::set_secondary_fill(float percentage, float time)
{
    if (percentage > 1.0f)
        overflow_ = true;
    secondary_target_ = math::clamp(percentage, 0.0f, 1.0f);
    if (time == 0.0f)
    {
        second_bar_.setSize(math::vector2f(bar_size_.x*percentage, bar_size_.y));
    }
    else
    {
        secondary_timer_ = 0.0f;
        secondary_t_max_ = time;
    }
    second_initial_width_ = second_bar_.getSize().x;
}
void double_bar::set_position(const math::vector2f& new_pos)
{
    background_.setPosition(new_pos - 1.0f);
    main_bar_.setPosition(new_pos);
    second_bar_.setPosition(new_pos);
}
math::vector2f double_bar::get_position() const
{
    return (main_bar_.getPosition());
}
void double_bar::update(double delta)
{
    main_full_ = false;
    second_full_ = false;
    if (main_timer_ < main_t_max_)
    {
        main_timer_ += delta;
        main_bar_.setSize(math::v2lerp(math::vector2f(main_initial_width_, bar_size_.y),
                                       math::vector2f(bar_size_.x*main_target_, bar_size_.y),
                                       math::clamp(main_timer_/main_t_max_, 0.0f, 1.0f)));
        if (gradient_enabled_)
            interpolate_color();
    }
    else if (main_timer_ != 0.0f && main_t_max_ != 0.0f)
    {
        main_timer_ = main_t_max_ = 0.0f;
    }
    if (main_bar_.getSize() == bar_size_.get_sf_vector())
        main_full_ = true;
    if (secondary_timer_ < secondary_t_max_)
    {
        secondary_timer_ += delta;
        second_bar_.setSize(math::v2lerp(math::vector2f(second_initial_width_, bar_size_.y),
                                       math::vector2f(bar_size_.x*secondary_target_, bar_size_.y),
                                       math::clamp(secondary_timer_/secondary_t_max_, 0.0f, 1.0f)));
    }
    else if (secondary_timer_ != 0.0f && secondary_t_max_ != 0.0f)
    {
        secondary_timer_ = secondary_t_max_ = 0.0f;
    }
    if (second_bar_.getSize() == bar_size_.get_sf_vector())
        second_full_ = true;
}
void double_bar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(background_, states);
    target.draw(second_bar_, states);
    target.draw(main_bar_, states);
}
void double_bar::enable_gradient(const std::vector<uint32_t>& colors)
{
    colors_ = colors;
    gradient_enabled_ = true;
    interpolate_color();
}

void double_bar::interpolate_color()
{
    float t = main_bar_.getLocalBounds().width / bar_size_.x;
    unsigned first = ceil(float(colors_.size() - 1)*t);
    unsigned second = floor(float(colors_.size() - 1)*t);
    if (first == second)
    {
        main_bar_.setFillColor(utils::argb(colors_[first]));
    }
    else
    {
        utils::argb c1(colors_[second]);
        utils::argb c2(colors_[first]);
        uint32_t c3 = 0x0;

        c3 = c3 | (uint32_t(c1.a() + (c2.a() - c1.a()) * t) << 24);
        c3 = c3 | (uint32_t(c1.r() + (c2.r() - c1.r()) * t) << 16);
        c3 = c3 | (uint32_t(c1.g() + (c2.g() - c1.g()) * t) << 8);
        c3 = c3 | uint32_t(c1.b() + (c2.b() - c1.b()) * t);

        main_bar_.setFillColor(utils::argb(c3));
    }
}
} // namespace gui
} // namespace overworld
} // namespace icarus
