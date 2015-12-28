
#ifndef ICARUS_ENCOUNTER_COMBAT_TEXT_HPP_
#define ICARUS_ENCOUNTER_COMBAT_TEXT_HPP_

#include <vector>
#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"
#include "icarus/math.hpp"

namespace icarus
{
namespace encounter
{
class combat_text : public sf::Drawable
{
private:
    struct text
    {
        sf::Text* text_;
        float time_left_;
        float fade_amount_;
        float fade_total_;
        math::vector2f move_vector_;
        void reduce_alpha(double delta)
        {
            fade_total_ = math::clamp(float(fade_total_ - fade_amount_ * delta), 0.0f, 255.0f);
            sf::Color new_color = text_->getColor();
            new_color.a = static_cast<uint8_t>(fade_total_);
            text_->setColor(new_color);
        }
    };
    float default_duration_;
    float default_speed_;
    float gravity_;
    unsigned default_size_;
    sf::Text::Style default_style_;
    bool fade_by_default_;
    std::vector<text> text_list_;

public:
    combat_text(float default_duration, float default_speed,
                unsigned default_size, sf::Text::Style default_style);
    ~combat_text();
    void update(double delta);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void add_text(const std::string& text,
                  const sf::Color& color,
                  const math::vector2f& position)
    { add_text(text, color, position, default_size_, default_style_,
               default_duration_, default_speed_); }

    void add_text(const std::string& text,
                  const sf::Color& color,
                  const math::vector2f& position,
                  unsigned text_size)
    { add_text(text, color, position, text_size, default_style_,
               default_duration_, default_speed_); }

    void add_text(const std::string& text,
                  const sf::Color& color,
                  const math::vector2f& position,
                  unsigned text_size,
                  sf::Text::Style style)
    { add_text(text, color, position, text_size, style,
               default_duration_, default_speed_); }

    void add_text(const std::string& text,
                  const sf::Color& color,
                  const math::vector2f& position,
                  unsigned text_size,
                  sf::Text::Style style,
                  float duration)
    { add_text(text, color, position, text_size, style,
               duration, default_speed_); }

    void add_text(const std::string& text,
                  const sf::Color& color,
                  const math::vector2f& position,
                  unsigned text_size,
                  sf::Text::Style style,
                  float duration,
                  float speed);

};
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_COMBAT_TEXT_HPP_
