#ifndef ICARUS_UI_CHECKBOX_HPP_
#define ICARUS_UI_CHECKBOX_HPP_

#include <SFML/Graphics.hpp>
#include "icarus/ui/sprite_sheet.hpp"

namespace icarus
{
namespace ui
{
class checkbox : public sf::Drawable
{
private:
    sprite_sheet box_;
    bool is_checked_;
    bool is_enabled_;

public:
    checkbox();
    virtual ~checkbox();

    void setup(utils::yth_node* data, bool initially_checked);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void set_position(const math::vector2f& pos) { box_.set_position(pos); }
    bool contains(const math::vector2f& pos) const { return box_.contains(pos); }
    void set_checked(bool status)
    {
        if (!is_enabled_) return;
        is_checked_ = status;
        box_.set_sprite(status ? "checked" : "unchecked");
    }
    bool is_checked() const { return is_checked_; }
    void toggle() { if (!is_enabled_) return; is_checked_ = !is_checked_; box_.set_sprite(is_checked_ ? "checked" : "unchecked");}
    void set_enabled(bool status) { is_enabled_ = status; }
};
}   // namespace ui
}   // namespace icarus

#endif // ICARUS_UI_CHECKBOX_HPP_
