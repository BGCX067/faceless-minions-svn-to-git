#ifndef ICARUS_OVERWORLD_GUI_HANDLER_HPP_
#define ICARUS_OVERWORLD_GUI_HANDLER_HPP_

#include <SFML/Graphics.hpp>

#include "icarus/utils/yth_handler.hpp"
#include "icarus/overworld/gui/encounter_dialog.hpp"
#include "icarus/overworld/gui/post_battle.hpp"
#include "icarus/overworld/gui/party_list.hpp"
#include "icarus/overworld/gui/party_member.hpp"
#include "icarus/overworld/gui/menu_bar.hpp"
#include "icarus/overworld/gui/inventory.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class gui_handler : public sf::Drawable
{
private:
    encounter_dialog enc_dialog_;
    post_battle pb_dialog_;
    party_list party_list_;
    gui::party_member party_screen_;
    menu_bar menu_;
    inventory inventory_;
    static gui_handler* instance_;

    gui_handler();
    gui_handler(gui_handler const&);
    void operator=(gui_handler const&);
public:
    virtual ~gui_handler();
    static gui_handler* get()
    {
        if (!instance_)
            instance_ = new gui_handler();
        return instance_;
    }

    void display_encounter_dialog(utils::yth_node* encounter_node);
    void display_postbattle_dialog(encounter::data* data);
    void display_party_list();
    void toggle_party_list();
    void display_party_screen(unsigned index);
    void handle_input(const std::vector<sf::Event>& input_events);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void reset_position();
    bool mouse_enabled(float x, float y);

    void toggle_inventory()
    {
        std::vector<item*> temp;
        if (inventory_.is_visible())
            close_inventory();
        else
            display_inventory(temp);
    }
    bool mouse_enabled(const math::vector2f& pos) { return mouse_enabled(pos.x, pos.y); }
    bool menu_contains(const math::vector2f& pos) const { return menu_.contains(pos); }
    void display_inventory(const std::vector<item*>& loot, encounter::data* data = NULL) { inventory_.setup_dialog(loot, data); }
    void close_party_screen() { party_screen_.close_dialog(); }
    void close_inventory() { inventory_.close_dialog(); }
    bool party_list_is_visible() const { return party_list_.is_visible(); }
    bool scrolling_enabled() const { return !(party_screen_.is_visible() || inventory_.is_visible()); }
    bool dialog_visible() const { return (enc_dialog_.is_visible() ||
                                          pb_dialog_.is_visible() ||
                                          party_screen_.is_visible() ||
                                          inventory_.is_visible()); }
    void load_dialog() { enc_dialog_.load_dialog(); }
    bool party_screen_visible() const { return party_screen_.is_visible(); }
    int party_screen_index() const { return party_screen_.get_selected_index(); }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_HANDLER_HPP_
