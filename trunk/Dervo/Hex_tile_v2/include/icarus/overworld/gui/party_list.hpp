#ifndef ICARUS_OVERWORLD_GUI_PARTY_LIST_HPP_
#define ICARUS_OVERWORLD_GUI_PARTY_LIST_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/overworld/gui/party_list_entry.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class party_list : public sf::Drawable
{
private:
    sf::Sprite background_;
    party_list_entry party_[6];
    bool visible_;
public:
    party_list();
    virtual ~party_list();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void setup_dialog();
    void hide_dialog();
    void handle_input(const std::vector<sf::Event>& input_events);
    void reset_position();
    bool is_visible() const { return visible_; }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_PARTY_LIST_HPP_
