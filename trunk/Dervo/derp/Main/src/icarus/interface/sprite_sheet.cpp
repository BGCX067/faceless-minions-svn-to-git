#include "icarus/interface/sprite_sheet.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace interface
{
sprite_sheet::sprite_sheet()
{

}
sprite_sheet::~sprite_sheet()
{

}
void sprite_sheet::load_sprite(utils::yth_node* sprite_node,
                 const std::string& sheet_path)
{
    for (unsigned i = 0; i < sprite_node->child_count(); ++i)
    {
        std::string key = sprite_node->child(i)->key();
        if (sprite_map_.count(key) > 0)
            continue;

        sprite_data temp_sprite;
        temp_sprite.start_x_ =        atoi(sprite_node->child(i)->child("start_x")->value().c_str());
        temp_sprite.start_y_ =        atoi(sprite_node->child(i)->child("start_y")->value().c_str());
        temp_sprite.frame_width_ =    atoi(sprite_node->child(i)->child("frame_width")->value().c_str());
        temp_sprite.frame_height_ =   atoi(sprite_node->child(i)->child("frame_height")->value().c_str());
        temp_sprite.origin_x_ =    atoi(sprite_node->child(i)->child("origin_x")->value().c_str());
        temp_sprite.origin_y_ =   atoi(sprite_node->child(i)->child("origin_y")->value().c_str());

        std::pair<std::string, sprite_data> new_pair(key, temp_sprite);
        sprite_map_.insert(new_pair);
    }

    sf::Texture* tex = NULL;
    if ((tex = resource_handler::get()->get_texture(sheet_path)))
        sprite_ = sf::Sprite(*tex);

    set_sprite(sprite_map_.begin()->first);
}
void sprite_sheet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite_, states);
}
void sprite_sheet::set_sprite(const std::string& new_sprite)
{
    if (sprite_map_.count(new_sprite) <= 0 ||
        current_sprite_ == new_sprite)
        return;

    current_sprite_ = new_sprite;
    int x = sprite_map_.at(new_sprite).start_x_;
    int y = sprite_map_.at(new_sprite).start_y_;

    sprite_.setTextureRect(sf::IntRect(x, y,
                                        sprite_map_.at(new_sprite).frame_width_,
                                        sprite_map_.at(new_sprite).frame_height_));
    sprite_.setOrigin(math::vector2f(sprite_map_.at(new_sprite).origin_x_, sprite_map_.at(new_sprite).origin_y_));
}
}   // namespace interface
}   // namespace icarus
