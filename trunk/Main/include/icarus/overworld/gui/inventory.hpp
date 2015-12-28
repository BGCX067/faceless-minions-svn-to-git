
#ifndef ICARUS_OVERWORLD_GUI_INVENTORY_HPP_
#define ICARUS_OVERWORLD_GUI_INVENTORY_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/overworld/gui/icon.hpp"
#include "icarus/overworld/party_member.hpp"
#include "icarus/encounter/data.hpp"
#include "icarus/encounter/visual_effect.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
namespace item_locations
{
enum value
{
    loot,
    backpack,
    inventory
};
}   // namespace item_locations
class inventory : public sf::Drawable
{
private:
    sf::Sprite background_;
    sf::Sprite close_button_;
    sf::Sprite loot_background_;
    sf::Sprite trash_;
    encounter::data* result_data_;

    icon* loot_icons_[6];
    item* loot_[6];
    struct inventory_member
    {
        party_member* member_;
        sf::Sprite portrait_;
        sf::Sprite background_;
        sf::Sprite deceased_;
        sf::Text name_;
        icon* inventory_icons_[6];
    };
    inventory_member party_[6];
    bool visible_;
    bool loot_visible_;
    bool drag_enabled_;
    struct dragged_item_
    {
        icon* icon_;
        item* item_;
        item_locations::value source_;
        unsigned source_index_;
        unsigned player_index_;
    } dragged_item_;

    void configure_tooltip(icon* ic, item* it);
    void update_icon_positions();

public:
    inventory();
    virtual ~inventory();

    void setup_dialog(const std::vector<item*>& loot, encounter::data* result_data = NULL);
    void close_dialog();
    void handle_input(const std::vector<sf::Event>& input_events);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool is_visible() const { return visible_; }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_INVENTORY_HPP_
