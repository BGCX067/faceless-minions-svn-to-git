#ifndef ICARUS_OVERWORLD_GUI_PARTY_LIST_ENTRY_HPP_
#define ICARUS_OVERWORLD_GUI_PARTY_LIST_ENTRY_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/overworld/gui/double_bar.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
class party_list_entry : public sf::Drawable
{
private:
    bool visible_;
    sf::Sprite background_;
    sf::Sprite portrait_;
    sf::Text name_;
    sf::Text class_level_;
    double_bar hp_bar_;
    double_bar xp_bar_;
    sf::Sprite unalloc_sprite_;
    sf::Sprite mask_;
    unsigned index_;

public:
    party_list_entry();
    virtual ~party_list_entry();

    void set_up(const std::string& name,
                const std::string& class_level,
                float hp, float xp,
                const std::string& class_name,
                unsigned index);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_position(const math::vector2f& pos);
    void set_visible(bool state) { visible_ = state; }
    bool is_visible() const { return visible_; }
    bool contains(const math::vector2f& pos) const { return background_.getGlobalBounds().contains(pos); }
};
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

#endif // ICARUS_OVERWORLD_GUI_PARTY_LIST_ENTRY_HPP_
