#ifndef ICARUS_ENCOUNTER_UI_SKILLBAR_HPP_
#define ICARUS_ENCOUNTER_UI_SKILLBAR_HPP_

#include <SFML/Graphics.hpp>

namespace icarus
{
namespace encounter
{
class state;
class character_hero;
namespace ui
{
class skillbar : public sf::Drawable
{
private:
    sf::Sprite buttons_[5];
    sf::Sprite background_;
    state* state_;
    character_hero* hero_;

public:
    skillbar(state* state);
    virtual ~skillbar();

    void handle_input(const sf::Event& input_event);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_selected_hero(character_hero* hero);
};
} // namespace ui
} // namespace encounter
} // namespace icarus

#endif // ICARUS_ENCOUNTER_UI_SKILLBAR_HPP_
