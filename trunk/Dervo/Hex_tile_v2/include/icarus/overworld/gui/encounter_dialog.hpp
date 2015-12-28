#ifndef ICARUS_OVERWORLD_GUI_ENCOUNTER_DIALOG_HPP_
#define ICARUS_OVERWORLD_GUI_ENCOUNTER_DIALOG_HPP_

#include <SFML/Graphics.hpp>

#include "icarus/utils/yth_handler.hpp"
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
namespace encounter_resolution
{
enum type
{
    peaceful,
    combat,
    combat_defence,
    combat_flank,
    aid_defending,
    aid_attacking,
    pay_heal,
    pay_member_random,
    pay_member_rogue,
    pay_member_warrior,
    pay_member_mage,
    pay_item
};
}
class encounter_dialog : public sf::Drawable
{
private:
    sf::Sprite background_;
    sf::Text title_;
    sf::Text description_;
    struct option
    {
        sf::Text option_text_;
        encounter_resolution::type result_;
    };
    std::vector<option> options_;
    std::vector<sf::Text> resolution_text_;
    sf::Text continue_text_;
    unsigned selected_option_;
    math::vector2f pos_offset_;
    bool visible_;
    bool resolved_;
    utils::yth_node* combat_node_;
    int experience_value_;
    int gold_income_;
    unsigned gold_expences_[3];

    void clean();
public:
    encounter_dialog();
    virtual ~encounter_dialog();

    void setup_dialog(utils::yth_node* encounter_node);
    void select_option(unsigned selected);
    void close_dialog();
    void handle_input(const std::vector<sf::Event>& input_events);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void reset_position();
    bool is_visible() const { return visible_; }
    void reset_gold_expence();
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_ENCOUNTER_DIALOG_HPP_
