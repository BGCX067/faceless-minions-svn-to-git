#ifndef ICARUS_ENCOUNTER_UI_ICON_HPP_
#define ICARUS_ENCOUNTER_UI_ICON_HPP_

#include <SFML/Graphics.hpp>

namespace icarus
{
namespace encounter
{
namespace ui
{
class icon : public sf::Drawable
{
private:
    sf::Sprite icon_;

    //sf::Image
public:
    icon();
    virtual ~icon();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
} // namespace ui
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_UI_ICON_HPP_
