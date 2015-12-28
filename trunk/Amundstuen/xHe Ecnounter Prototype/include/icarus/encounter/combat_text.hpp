
#ifndef ICARUS_ENCOUNTER_COMBAT_TEXT_HPP_
#define ICARUS_ENCOUNTER_COMBAT_TEXT_HPP_

#include <vector>
#include <SFML/Graphics.hpp>
#include "icarus/math/vector2.hpp"

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
        uint8_t fade_amount_;
        float speed_;
        void reduce_alpha(float delta)
        {
            sf::Color new_color = text_->getColor();
            uint8_t amount = static_cast<uint8_t>(ceil(fade_amount_ * delta));
            if (static_cast<int>(new_color.a) - amount < 0)
                new_color.a = 0;
            else
                new_color.a -= amount;
            text_->setColor(new_color);
        }
    };
    float default_duration_;
    float default_speed_;
    unsigned default_size_;
    sf::Text::Style default_style_;
    bool fade_by_default_;
    std::vector<text> text_list_;

public:
    combat_text(float default_duration, float default_speed,
                unsigned default_size, sf::Text::Style default_style);
    ~combat_text();
    void update(float delta);
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
