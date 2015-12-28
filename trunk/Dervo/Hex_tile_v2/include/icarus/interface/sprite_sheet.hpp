#ifndef ICARUS_INTERFACE_SPRITE_SHEET_HPP_
#define ICARUS_INTERFACE_SPRITE_SHEET_HPP_

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "icarus/utils/yth_handler.hpp"
#include "icarus/math/vector2.hpp"

namespace icarus
{
namespace interface
{
class sprite_sheet : public sf::Drawable
{
private:
    struct sprite_data
    {
        int start_x_;
        int start_y_;
        int frame_width_;
        int frame_height_;
        int origin_x_;
        int origin_y_;
    };
    std::string current_sprite_;
    std::unordered_map<std::string, sprite_data> sprite_map_;
    sf::Sprite sprite_;
public:
    sprite_sheet();
    virtual ~sprite_sheet();
    void load_sprite(utils::yth_node* sprite_node,
                     const std::string& sheet_path);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void set_sprite(const std::string& new_sprite);
    sf::FloatRect get_bounds() const { return sprite_.getGlobalBounds(); }
    void set_position(const math::vector2f& pos) { sprite_.setPosition(pos); }
    bool contains(const math::vector2f& pos) const
    { return sprite_.getGlobalBounds().contains(pos); }
};
}   // namespace interface
}   // namespace icarus

#endif // ICARUS_INTERFACE_SPRITE_SHEET_HPP_
