#ifndef ICARUS_UI_SCREEN_FADE_HPP_
#define ICARUS_UI_SCREEN_FADE_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/utils.hpp"

namespace icarus
{
namespace ui
{
class screen_fade : public sf::Drawable
{
private:
    sf::RectangleShape screen_;
    float timer_;
    float duration_;
    bool fade_in_;
    bool active_;

public:
    screen_fade(const utils::rgb& color, float duration, bool start_opaque);
    virtual ~screen_fade();

    void reset(bool fade_in);
    bool update(double delta);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool is_active() const { return active_; }
    void set_duration(float duration) { duration_ = duration; }
};
}
}

#endif // ICARUS_UI_SCREEN_FADE_HPP_
