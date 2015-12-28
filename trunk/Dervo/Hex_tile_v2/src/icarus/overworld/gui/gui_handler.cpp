#include "icarus/overworld/gui/gui_handler.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
gui_handler* gui_handler::instance_ = NULL;

gui_handler::gui_handler()
{

}
gui_handler::~gui_handler()
{

}

void gui_handler::display_encounter_dialog(utils::yth_node* encounter_node)
{
    enc_dialog_.setup_dialog(encounter_node);
    enc_dialog_.reset_position();
}
void gui_handler::display_postbattle_dialog(encounter::data* data)
{
    pb_dialog_.setup_dialog(data);
    pb_dialog_.reset_position();
}
void gui_handler::display_party_list()
{
    party_list_.setup_dialog();
    party_list_.reset_position();
}
void gui_handler::toggle_party_list()
{
    if (party_list_.is_visible())
    {
        party_list_.hide_dialog();
    }
    else
    {
        party_list_.setup_dialog();
        party_list_.reset_position();
    }
}
void gui_handler::display_party_screen(unsigned index)
{
    party_screen_.setup_dialog(index);
}
void gui_handler::handle_input(const std::vector<sf::Event>& input_events)
{
    menu_.handle_input(input_events);
    if (party_list_.is_visible())
        party_list_.handle_input(input_events);
    if (party_screen_.is_visible())
        party_screen_.handle_input(input_events);
    else if (enc_dialog_.is_visible())
        enc_dialog_.handle_input(input_events);
    else if (pb_dialog_.is_visible())
        pb_dialog_.handle_input(input_events);
}
void gui_handler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(menu_, states);
    if (party_list_.is_visible())
        target.draw(party_list_, states);
    if (party_screen_.is_visible())
        target.draw(party_screen_, states);
    else if (enc_dialog_.is_visible())
        target.draw(enc_dialog_, states);
    else if (pb_dialog_.is_visible())
        target.draw(pb_dialog_, states);
}
void gui_handler::reset_position()
{
    menu_.reset_position();
    if (party_list_.is_visible())
        party_list_.reset_position();
    if (enc_dialog_.is_visible())
        enc_dialog_.reset_position();
    if (pb_dialog_.is_visible())
        pb_dialog_.reset_position();
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
