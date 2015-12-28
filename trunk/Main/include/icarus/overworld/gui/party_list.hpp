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
    bool drag_party_member_;
    bool start_drag_;
    unsigned dragged_index_;
    bool active_;
    math::vector2f up_pos_;
    math::vector2f down_pos_;
    math::vector2f current_pos_;
    float pos_t_;
    float movement_;
public:
    party_list();
    virtual ~party_list();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void setup_dialog();
    void hide_dialog();
    void handle_input(const std::vector<sf::Event>& input_events);
    void reset_position();
    bool swap_members(unsigned a, unsigned b);
    bool is_visible() const { return visible_; }
    bool contains(const math::vector2f& pos) const { return background_.getGlobalBounds().contains(pos); }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_PARTY_LIST_HPP_
