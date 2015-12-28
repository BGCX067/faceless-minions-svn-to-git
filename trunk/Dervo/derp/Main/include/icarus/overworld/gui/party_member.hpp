#ifndef ICARUS_OVERWORLD_GUI_PARTY_MEMBER_HPP_
#define ICARUS_OVERWORLD_GUI_PARTY_MEMBER_HPP_

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
namespace text_column
{
enum type
{
    label,
    value
};
}
class party_member : public sf::Drawable
{
private:
    sf::Sprite background_;
    // HACKATHON GO
    // name
    sf::Text name_lvl_class_;

    // attributes
    sf::Text attributes_[3][2];
    icon attrib_plus_[3];
    sf::Text attrib_unalloc_;

    // stats
    sf::Text stats_ka_[2];
    sf::Text stats_bc_[2];
    sf::Text stats_dmg_[2];
    sf::Text stats_hit_[2];

    // character and inventory
    sf::Sprite character_;
    sf::Sprite inventory_[4];

    // talents
    icon talents_[5][2];

    //skills
    icon skills_[4];
    encounter::visual_effect* autocast_indicators_[4];
    sf::Text skill_pts_;
    // HACKATHON END

    bool visible_;
    overworld::party_member* selected_member_;
    encounter::hero_data data_;

public:
    party_member();
    virtual ~party_member();

    void setup_dialog(unsigned party_index);
    void close_dialog();
    void handle_input(const std::vector<sf::Event>& input_events);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool is_visible() const { return visible_; }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_PARTY_MEMBER_HPP_
