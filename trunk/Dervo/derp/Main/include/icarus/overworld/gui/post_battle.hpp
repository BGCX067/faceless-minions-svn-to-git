#ifndef ICARUS_OVERWORLD_GUI_POST_BATTLE_HPP_
#define ICARUS_OVERWORLD_GUI_POST_BATTLE_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/overworld/gui/double_bar.hpp"
#include "icarus/encounter/data.hpp"

namespace icarus
{
namespace overworld
{
class party_member;
namespace gui
{
class post_battle : public sf::Drawable
{
private:
    sf::Sprite background_;
    sf::Text title_;
    struct member
    {
        sf::Text name_;
        sf::Text class_level_;
        sf::Text kills_assists_;
        sf::Text dmg_done_ff_;
        sf::Text dmg_taken_;
        double_bar hp_bar_;
        double_bar xp_bar_;
        party_member* ptr_;
        int xp_given_;
        int xp_total_;
        unsigned displayed_level_;
        member(const math::vector2f& bar_size,
               const sf::Color& bar_bg,
               const sf::Color& hp_main,
               const sf::Color& hp_second,
               const sf::Color& xp_main,
               const sf::Color& xp_second)
        :
            hp_bar_(hp_main, hp_second, bar_bg, bar_size),
            xp_bar_(xp_main, xp_second, bar_bg, bar_size)
        {
        }
    };
    std::vector<member> members_;
    sf::Text continue_text_;
    math::vector2f pos_offset_;
    bool visible_;

    void clean();
public:
    post_battle();
    virtual ~post_battle();

    void setup_dialog(encounter::data* data);
    void close_dialog();
    void handle_input(const std::vector<sf::Event>& input_events);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void reset_position();
    bool is_visible() const { return visible_; }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus


#endif // ICARUS_OVERWORLD_GUI_POST_BATTLE_HPP_
