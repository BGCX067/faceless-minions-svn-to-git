#ifndef ICARUS_ENCOUNTER_UI_PORTRAIT_HPP_
#define ICARUS_ENCOUNTER_UI_PORTRAIT_HPP_

#include "icarus/encounter/ui/hp_bar.hpp"

namespace icarus
{
namespace encounter
{
namespace ui
{
class portrait : public sf::Drawable
{
private:
    sf::Sprite frame_;
    sf::Sprite portrait_;
    sf::Text name_;
    hp_bar health_;
    bar cast_bar_;
    sf::CircleShape skill_icons_[5];
    bool icon_states_[5];
    unsigned offset_x_;
    unsigned offset_y_;

    void set_position(unsigned index);
public:
    portrait(unsigned index, const std::string& name, const sf::Sprite& port);
    virtual ~portrait();

    void set_skill_ready(unsigned index, bool ready);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    sf::Sprite* get_portrait_ptr();

    bar& get_cast_bar() { return cast_bar_; }
    hp_bar& get_hp_bar() { return health_; }
};
}   // namespace ui
}   // namespace encounter
}   // namespace icarus
#endif // ICARUS_ENCOUNTER_UI_PORTRAIT_HPP_
