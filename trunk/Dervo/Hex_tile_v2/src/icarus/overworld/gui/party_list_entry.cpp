#include "icarus/overworld/gui/party_list_entry.hpp"
#include "icarus/utils.hpp"
#include "icarus/math/vector2.hpp"
#include "icarus/resource_handler.hpp"

#include "icarus/state_manager.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
party_list_entry::party_list_entry()
:
    visible_(false),
    hp_bar_(utils::rgb(0x38C238),
            utils::rgb(0x333333),
            utils::argb(0x00000000),
            math::vector2f(130.0f, 18.0f)),
    xp_bar_(utils::rgb(0xF5CB11),
            utils::rgb(0x333333),
            utils::argb(0x00000000),
            math::vector2f(135.0f, 3.0f)),
    unalloc_(false)
{
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("party_list");
    std::string tex_path = dialog_root->child("member_background")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_ = sf::Sprite(*tex_ptr);

    tex_path = dialog_root->child("member_unalloc")->value();
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        unalloc_sprite_ = sf::Sprite(*tex_ptr);

    tex_path = dialog_root->child("member_mask")->value();
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        mask_ = sf::Sprite(*tex_ptr);

    name_.setCharacterSize(15);
    name_.setFont(*resource_handler::get()->get_font("text"));
    name_.setColor(utils::rgb(0x190701));
    class_level_.setCharacterSize(14);
    class_level_.setFont(*resource_handler::get()->get_font("text"));
    class_level_.setColor(utils::rgb(0x190701));
    hp_bar_.set_secondary_fill(1.0);
    std::vector<uint32_t> colors;
    colors.push_back(0xFFFF0000);
    colors.push_back(0xFFFF9900);
    colors.push_back(0xFF38C238);
    hp_bar_.enable_gradient(colors);
    xp_bar_.set_secondary_fill(1.0);
}
party_list_entry::~party_list_entry()
{

}

void party_list_entry::set_up(const std::string& name,
            const std::string& class_level,
            float hp, float xp, bool unalloc)
{
    name_.setString(name);
    class_level_.setString(class_level);
    hp_bar_.set_main_fill(hp);
    xp_bar_.set_main_fill(xp);
    visible_ = true;
}
void party_list_entry::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_)
        return;
    target.draw(background_, states);
    target.draw(hp_bar_, states);
    target.draw(xp_bar_, states);
    target.draw(mask_, states);
    if (unalloc_)
        target.draw(unalloc_sprite_, states);
    target.draw(name_, states);
    target.draw(class_level_, states);
}

void party_list_entry::set_position(const math::vector2f& pos)
{
    background_.setPosition(pos);
    mask_.setPosition(pos);
    utils::yth_node* node = resource_handler::get()->get_root_node("ui")->child("party_list");
    name_.setPosition(pos + math::vector2f(atof(node->child("label_x")->value().c_str()),
                                           atof(node->child("label_y")->value().c_str())));
    class_level_.setPosition(pos + math::vector2f(atof(node->child("class_label_x")->value().c_str()),
                                           atof(node->child("class_label_y")->value().c_str())));
    hp_bar_.set_position(pos + math::vector2f(atof(node->child("hp_x")->value().c_str()),
                                           atof(node->child("hp_y")->value().c_str())));
    xp_bar_.set_position(pos + math::vector2f(atof(node->child("xp_x")->value().c_str()),
                                           atof(node->child("xp_y")->value().c_str())));
    unalloc_sprite_.setPosition(pos + math::vector2f(atof(node->child("unalloc_x")->value().c_str()),
                                           atof(node->child("unalloc_y")->value().c_str())));
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
