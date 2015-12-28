#include "icarus/overworld/gui/party_member.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/utils.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
#include "icarus/overworld/item.hpp"
#include "icarus/sound.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
party_member::party_member()
:
    visible_(false),
    selected_member_(NULL)
{
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("party_member_dialog");
    std::string tex_path = dialog_root->child("background")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_ = sf::Sprite(*tex_ptr);
    tex_path = dialog_root->child("close_button")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        close_button_ = sf::Sprite(*tex_ptr);

    name_lvl_class_.setCharacterSize(25);
    name_lvl_class_.setFont(*resource_handler::get()->get_font("text"));

    tex_ptr = resource_handler::get()->get_texture(dialog_root->child("attrib_plus")->value());
    for (unsigned i = 0; i < 3; ++i)
    {
        attributes_[i][0].setCharacterSize(18);
        attributes_[i][0].setFont(*resource_handler::get()->get_font("text"));
        attributes_[i][0].setColor(utils::rgb(0x190701));
        attributes_[i][1].setCharacterSize(18);
        attributes_[i][1].setFont(*resource_handler::get()->get_font("text"));
        attributes_[i][1].setColor(utils::rgb(0x190701));
        if (tex_ptr != NULL)
            attrib_plus_[i].set_texture(*tex_ptr);
    }
    attributes_[attribs::intelligence][text_column::label].setString("Intelligence:");
    attributes_[attribs::strength][text_column::label].setString("Strength:");
    attributes_[attribs::agility][text_column::label].setString("Agility:");
    attrib_unalloc_.setCharacterSize(25);
    attrib_unalloc_.setFont(*resource_handler::get()->get_font("numbers"));

    for (unsigned i = 0; i < 2; ++i)
    {
        stats_ka_[i].setCharacterSize(16);
        stats_ka_[i].setFont(*resource_handler::get()->get_font("text"));
        stats_ka_[i].setColor(utils::rgb(0x190701));
        stats_bc_[i].setCharacterSize(16);
        stats_bc_[i].setFont(*resource_handler::get()->get_font("text"));
        stats_bc_[i].setColor(utils::rgb(0x190701));
        stats_dmg_[i].setCharacterSize(16);
        stats_dmg_[i].setFont(*resource_handler::get()->get_font("text"));
        stats_dmg_[i].setColor(utils::rgb(0x190701));
        stats_hit_[i].setCharacterSize(16);
        stats_hit_[i].setFont(*resource_handler::get()->get_font("text"));
        stats_hit_[i].setColor(utils::rgb(0x190701));
    }
    stats_ka_[text_column::label].setString("Total Kills/Assists:");
    stats_bc_[text_column::label].setString("Battles Fought:");
    stats_dmg_[text_column::label].setString("Average Damage Dealt/Taken:");
    stats_hit_[text_column::label].setString("Largest Hit:");

    skill_pts_.setCharacterSize(25);
    skill_pts_.setColor(utils::rgb(0x190701));
    skill_pts_.setFont(*resource_handler::get()->get_font("numbers"));

    for (unsigned i = 0; i < 4; ++i)
        autocast_indicators_[i] = new encounter::visual_effect(math::vector2f(0.0f, 0.0f),
                        resource_handler::get()->get_root_node("effects")->child("ui")->child("autocast"),
                        true, 0.0f);
}
party_member::~party_member()
{

}

void party_member::setup_dialog(unsigned party_index)
{
    selected_member_ = party_handler::get()->get_member(party_index);
    selected_member_->generate_hero_data(data_);
    // generics
    attributes_[attribs::intelligence][text_column::value].setString(utils::to_str(data_.intelligence_));
    attributes_[attribs::strength][text_column::value].setString(utils::to_str(data_.strength_));
    attributes_[attribs::agility][text_column::value].setString(utils::to_str(data_.agility_));

    stats_ka_[text_column::value].setString(utils::to_str(data_.kills_) + "/" + utils::to_str(data_.assists_));
    unsigned fought = selected_member_->get_total_battles();
    stats_bc_[text_column::value].setString(utils::to_str(fought));
    int done = (fought > 0 ? data_.damage_done_ / fought : 0);
    int taken = (fought > 0 ? data_.damage_taken_ / fought : 0);
    stats_dmg_[text_column::value].setString(utils::to_str(done) + "/" + utils::to_str(taken));
    stats_hit_[text_column::value].setString(utils::to_str(data_.largest_given_));

    attrib_unalloc_.setString(utils::to_str(selected_member_->get_attrib_pts()));
    skill_pts_.setString(utils::to_str(selected_member_->get_skill_pts()));
    if (selected_member_->get_attrib_pts() > 0)
    {
        for (unsigned j = 0; j < 3; ++j)
            attrib_plus_[j].set_active(true);
    }
    else
    {
        for (unsigned j = 0; j < 3; ++j)
            attrib_plus_[j].set_active(false);
    }

    for (unsigned i = 0; i < 3; ++i)
    {
        attrib_plus_[i].set_tt_data(resource_handler::get()->get_root_node("ui")->child("attribute_tooltips")->child(i));
        attrib_plus_[i].set_tt_enabled(true);
    }

    // specifics
    utils::yth_node* class_node = resource_handler::get()->get_root_node("classes");
    switch (selected_member_->get_class())
    {
    case encounter::hero_class::warrior:
    {
        name_lvl_class_.setString(selected_member_->get_name() + " the level " +
                                  utils::to_str(selected_member_->get_level()) + " " +
                                  "Ironclad");
        class_node = class_node->child("warrior");
        break;
    }
    case encounter::hero_class::mage:
    {
        name_lvl_class_.setString(selected_member_->get_name() + " the level " +
                                  utils::to_str(selected_member_->get_level()) + " " +
                                  "Alchemist");
        class_node = class_node->child("mage");
        break;
    }
    case encounter::hero_class::rogue:
    {
        name_lvl_class_.setString(selected_member_->get_name() + " the level " +
                                  utils::to_str(selected_member_->get_level()) + " " +
                                  "Quicksilver");
        class_node = class_node->child("rogue");
        break;
    }
    }
    std::string tex_path = "";
    sf::Texture* tex_ptr = NULL;
    utils::yth_node* icon_root = resource_handler::get()->get_root_node("ui")->child("icons");
    for (unsigned i = 0; i < 4; ++i)
    {
        tex_path = "images/ui/icons/"+icon_root->child(class_node->child("skill", i+1)->value())->value();
        if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
            skills_[i].set_texture(*tex_ptr);
        tex_ptr = NULL;
        skills_[i].set_active((selected_member_->get_skill_lvl(i) > 0));
        skills_[i].set_highlight(selected_member_->get_skill_pts() > 0 && selected_member_->can_level_skill(i));

        skills_[i].set_tt_data(resource_handler::get()->get_root_node("skills")->child(class_node->child("skill", i+1)->value())->child("tooltip"));
        skills_[i].set_tt_enabled(true);
        autocast_indicators_[i]->reset();

        item* t_item = selected_member_->get_item_at(i, false);
        if (t_item)
        {
            inventory_[i].set_texture(*resource_handler::get()->get_texture(resource_handler::get()->get_root_node("ui")->child("item_icons")->child(t_item->get_icon_name())->value()));
            inventory_[i].set_visible(true);
            inventory_[i].set_tt_enabled(true);
            inventory_[i].set_tt_type(ui::tooltip_type::item);
            inventory_[i].set_tt_data(t_item->get_tt_data_node());
        }
        else
        {
            inventory_[i].set_visible(false);
            inventory_[i].set_tt_enabled(false);
        }
    }

    // talents go

    // positioning
    utils::yth_node* pos_node = resource_handler::get()->get_root_node("ui")->child("party_member_dialog");
    math::vector2f base_pos;
    if (state_manager::get()->get_hd_status())
    {
        base_pos = math::vector2f(state_manager::get()->get_dimensions() - math::vector2f(background_.getLocalBounds().width, background_.getLocalBounds().height))/2;
    }
    else
    {
        base_pos = math::vector2f(atof(pos_node->child("x_pos")->value().c_str()),
                                  atof(pos_node->child("y_pos")->value().c_str()));
    }
    math::vector2f origin(input_handler::get()->convert_mouse_pos(math::vector2f(0.0f, 0.0f)));
    base_pos += origin;
    background_.setPosition(base_pos);

    math::vector2f offset(atof(pos_node->child("name_offset_x")->value().c_str()),
                          atof(pos_node->child("name_offset_y")->value().c_str()));
    name_lvl_class_.setPosition(base_pos + offset);

    offset = math::vector2f(atof(pos_node->child("character")->child("portrait_x")->value().c_str()),
                            atof(pos_node->child("character")->child("portrait_y")->value().c_str()));
    character_.setPosition(base_pos + offset);
    offset = math::vector2f(atof(pos_node->child("character")->child("inventory_x")->value().c_str()),
                            atof(pos_node->child("character")->child("inventory_y")->value().c_str()));
    float spacing = atof(pos_node->child("character")->child("inventory_spacing")->value().c_str());
    for (unsigned i = 0; i < 4; ++i)
    {
        inventory_[i].set_position(base_pos + offset + (math::vector2f(0.0f, spacing) * i));
    }
    offset = math::vector2f(atof(pos_node->child("stats")->child("attrib_text_x")->value().c_str()),
                            atof(pos_node->child("stats")->child("attrib_text_y")->value().c_str()));
    spacing = atof(pos_node->child("stats")->child("attrib_spacing")->value().c_str());
    float alt_x = atof(pos_node->child("stats")->child("attrib_val_x")->value().c_str());
    math::vector2f offset_alt(atof(pos_node->child("stats")->child("attrib_btn_x")->value().c_str()),
                              atof(pos_node->child("stats")->child("attrib_btn_y")->value().c_str()));
    for (unsigned i = 0; i < 3; ++i)
    {
        attributes_[i][0].setPosition(base_pos + offset + (math::vector2f(0.0f, spacing) * i));
        attributes_[i][1].setPosition(base_pos + math::vector2f(alt_x, offset.y) + (math::vector2f(0.0f, spacing) * i));
        attrib_plus_[i].set_position(base_pos + offset_alt + (math::vector2f(0.0f, spacing) * i));
    }
    offset = math::vector2f(atof(pos_node->child("stats")->child("stat_text_x")->value().c_str()),
                            atof(pos_node->child("stats")->child("stat_text_y")->value().c_str()));
    spacing = atof(pos_node->child("stats")->child("stat_spacing")->value().c_str());
    alt_x = atof(pos_node->child("stats")->child("stat_val_x")->value().c_str());
    stats_ka_[0].setPosition(base_pos + offset + (math::vector2f(0.0f, spacing) * 0));
    stats_bc_[0].setPosition(base_pos + offset + (math::vector2f(0.0f, spacing) * 1));
    stats_dmg_[0].setPosition(base_pos + offset + (math::vector2f(0.0f, spacing) * 2));
    stats_hit_[0].setPosition(base_pos + offset + (math::vector2f(0.0f, spacing) * 3));
    stats_ka_[1].setPosition(base_pos + math::vector2f(alt_x, offset.y) + (math::vector2f(0.0f, spacing) * 0));
    stats_bc_[1].setPosition(base_pos + math::vector2f(alt_x, offset.y) + (math::vector2f(0.0f, spacing) * 1));
    stats_dmg_[1].setPosition(base_pos + math::vector2f(alt_x, offset.y) + (math::vector2f(0.0f, spacing) * 2));
    stats_hit_[1].setPosition(base_pos + math::vector2f(alt_x, offset.y) + (math::vector2f(0.0f, spacing) * 3));
    offset = math::vector2f(atof(pos_node->child("stats")->child("unalloc_x")->value().c_str()),
                            atof(pos_node->child("stats")->child("unalloc_y")->value().c_str()));
    attrib_unalloc_.setPosition(base_pos + offset);
    offset = math::vector2f(atof(pos_node->child("talents")->child("col1_x")->value().c_str()),
                            atof(pos_node->child("talents")->child("col_y")->value().c_str()));
    spacing = atof(pos_node->child("talents")->child("spacing")->value().c_str());
    alt_x = atof(pos_node->child("talents")->child("col2_x")->value().c_str());
    for (unsigned i = 0; i < 5; ++i)
    {
        talents_[i][0].set_position(base_pos + offset + (math::vector2f(0.0f, spacing) * i));
        talents_[i][1].set_position(base_pos + math::vector2f(alt_x, offset.y) + (math::vector2f(0.0f, spacing) * i));
    }
    offset = math::vector2f(atof(pos_node->child("skills")->child("initial_x")->value().c_str()),
                            atof(pos_node->child("skills")->child("even_y")->value().c_str()));
    spacing = atof(pos_node->child("skills")->child("spacing")->value().c_str());
    alt_x = atof(pos_node->child("skills")->child("odd_y")->value().c_str());
    for (unsigned i = 0; i < 4; ++i)
    {
        skills_[i].set_position(base_pos + math::vector2f(offset.x, (i%2?alt_x:offset.y)) + (math::vector2f(spacing, 0.0f) * i));
        autocast_indicators_[i]->set_world_pos(skills_[i].get_position() + (skills_[i].get_dimensions()/2) + math::vector2f(5.0f, 0.0f));
    }
    offset = math::vector2f(atof(pos_node->child("skills")->child("unalloc_x")->value().c_str()),
                            atof(pos_node->child("skills")->child("unalloc_y")->value().c_str()));
    skill_pts_.setPosition(base_pos + offset);

    offset = math::vector2f(atof(pos_node->child("close_button_x")->value().c_str()),
                            atof(pos_node->child("close_button_y")->value().c_str()));
    close_button_.setPosition(base_pos + offset);

    visible_ = true;
}
void party_member::close_dialog()
{
    sound::get()->play_sfx("close_dialog");
    visible_ = false;
    for (unsigned i = 0; i < 4; ++i)
    {
        skills_[i].set_tt_enabled(false);
        inventory_[i].set_tt_enabled(false);
    }
    for (unsigned i = 0; i < 3; ++i)
        attrib_plus_[i].set_tt_enabled(false);

    party_handler::get()->get_state().state_ = unsigned(utils::encounter_state::no_encounter);
    state_manager::get()->save_game();
}
void party_member::handle_input(const std::vector<sf::Event>& input_events)
{
    if (!visible_ || selected_member_ == NULL)
        return;
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch (input_events[i].type)
        {
        case sf::Event::MouseButtonReleased:
            {
                math::vector2f mouse_pos(input_events[i].mouseButton.x,
                                         input_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);
                if (selected_member_->get_attrib_pts() > 0)
                {
                    for (unsigned k = 0; k < 3; ++k)
                    {
                        if (attrib_plus_[k].contains(mouse_pos) &&
                            selected_member_->spend_attrib_point(k))
                        {
                            if (selected_member_->get_attrib_pts() <= 0)
                            {
                                for (unsigned j = 0; j < 3; ++j)
                                    attrib_plus_[j].set_active(false);
                            }
                            attributes_[k][1].setString(utils::to_str(selected_member_->get_attribute(k)));
                            attrib_unalloc_.setString(utils::to_str(selected_member_->get_attrib_pts()));
                        }
                    }
                }
                for (unsigned k = 0; k < 4; ++k)
                {
                    if (skills_[k].contains(mouse_pos))
                    {
                        if (input_events[i].mouseButton.button == sf::Mouse::Left &&
                            selected_member_->get_skill_pts() > 0 &&
                            selected_member_->spend_skill_point(k))
                        {
                            if (selected_member_->get_skill_pts() <= 0)
                            {
                                for (unsigned j = 0; j < 4; ++j)
                                {
                                    skills_[j].set_highlight(false);
                                }
                            }
                            skills_[k].set_highlight(selected_member_->can_level_skill(k));
                            skills_[k].set_active(true);
                            skill_pts_.setString(utils::to_str(selected_member_->get_skill_pts()));
                        }
                        else if (input_events[i].mouseButton.button == sf::Mouse::Right)
                        {
                            selected_member_->set_autocast(k+1, !selected_member_->get_autocast(k+1));
                        }
                    }
                }
                if (close_button_.getGlobalBounds().contains(mouse_pos))
                    gui_handler::get()->close_party_screen();
                break;
            }
        default: break;
        }
    }
    double delta = state_manager::get()->get_delta();
    for (unsigned i = 0; i < 4; ++i)
        autocast_indicators_[i]->update(delta);
}
void party_member::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_ || !selected_member_)
        return;

    target.draw(background_, states);
    target.draw(name_lvl_class_, states);
    for (unsigned i = 0; i < 5; ++i)
    {
        if (i < 2)
        {
            target.draw(stats_ka_[i], states);
            target.draw(stats_bc_[i], states);
            target.draw(stats_dmg_[i], states);
            target.draw(stats_hit_[i], states);
        }
        if (i < 3)
        {
            target.draw(attributes_[i][0], states);
            target.draw(attributes_[i][1], states);
            target.draw(attrib_plus_[i], states);
        }
        if (i < 4)
        {
            target.draw(inventory_[i], states);
            target.draw(skills_[i], states);
            if (selected_member_->get_autocast(i+1))
                target.draw(*autocast_indicators_[i], states);
        }
        target.draw(talents_[i][0], states);
        target.draw(talents_[i][1], states);
    }
    target.draw(attrib_unalloc_, states);
    target.draw(character_, states);
    target.draw(skill_pts_, states);
    target.draw(close_button_, states);
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
