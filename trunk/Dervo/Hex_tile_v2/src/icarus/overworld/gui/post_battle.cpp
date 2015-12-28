#include "icarus/overworld/gui/post_battle.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/input_handler.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
void post_battle::clean()
{
    members_.clear();
    visible_ = false;
    continue_text_.setColor(utils::rgb(0xFFFFFF));
}
post_battle::post_battle()
:
    visible_(false)
{
    utils::yth_node* ui_root = resource_handler::get()->get_root_node("ui");
    std::string bg_path = ui_root->child("post_battle_dialog")->child("background")->value();
    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(bg_path))
        background_ = sf::Sprite(*bg_tex);

    //TODO:: remove magic numbers?
    title_.setCharacterSize(25);
    title_.setFont(*resource_handler::get()->get_font("text"));
    title_.setString("Post-Battle summary:");

    continue_text_.setCharacterSize(20);
    continue_text_.setFont(*resource_handler::get()->get_font("text"));
    continue_text_.setString("1. Continue...");
}
post_battle::~post_battle()
{
}

void post_battle::setup_dialog(encounter::data* data)
{
    party_member* member_ptr = NULL;
    unsigned survivors = 0;
    for (unsigned i = 0; i < data->player_party_.size(); ++i)
        survivors += ceil(data->player_party_[i].current_health_);

    for (unsigned i = 0; i < data->player_party_.size(); ++i)
    {
        member new_member(math::vector2f(150.0f, 8.0f),  // bar size
                          utils::rgb(0x333333),     // bar bg
                          utils::rgb(0x22DD22),     // hp main
                          utils::rgb(0xDD2222),     // hp second
                          utils::rgb(0xF5CB11),     // xp main
                          utils::rgb(0xFFE261));    // xp second
        member_ptr = party_handler::get()->get_member(data->player_party_[i].index_);
        new_member.name_.setString(member_ptr->get_name(16));
        new_member.name_.setCharacterSize(18);
        new_member.name_.setFont(*resource_handler::get()->get_font("text"));
        switch(data->player_party_[i].class_)
        {
        case encounter::hero_class::warrior:
            new_member.class_level_.setString("Level "+utils::to_str(member_ptr->get_level())+" Ironclad");
            break;
        case encounter::hero_class::rogue:
            new_member.class_level_.setString("Level "+utils::to_str(member_ptr->get_level())+" Quicksilver");
            break;
        case encounter::hero_class::mage:
            new_member.class_level_.setString("Level "+utils::to_str(member_ptr->get_level())+" Alchemist");
            break;
        }
        new_member.displayed_level_ = member_ptr->get_level();
        new_member.class_level_.setCharacterSize(12);
        new_member.class_level_.setFont(*resource_handler::get()->get_font("text"));
        new_member.kills_assists_.setString("Kills/Assists: "+utils::to_str(data->player_party_[i].kills_)+ "/" +
                                            utils::to_str(data->player_party_[i].assists_));
        new_member.kills_assists_.setCharacterSize(14);
        new_member.kills_assists_.setFont(*resource_handler::get()->get_font("text"));
        new_member.dmg_done_ff_.setString("Damage done (friendly): "+utils::to_str(data->player_party_[i].damage_done_)+ "(" +
                                            utils::to_str(data->player_party_[i].friendly_fire_) + ")");
        new_member.dmg_done_ff_.setCharacterSize(14);
        new_member.dmg_done_ff_.setFont(*resource_handler::get()->get_font("text"));
        new_member.dmg_taken_.setString("Damage taken: "+utils::to_str(data->player_party_[i].damage_taken_));
        new_member.dmg_taken_.setCharacterSize(14);
        new_member.dmg_taken_.setFont(*resource_handler::get()->get_font("text"));

        new_member.hp_bar_.set_main_fill(member_ptr->get_current_hp());
        new_member.hp_bar_.set_main_fill(data->player_party_[i].current_health_, 2.0f);
        new_member.hp_bar_.set_secondary_fill(member_ptr->get_current_hp());

        new_member.ptr_ = member_ptr;
        new_member.xp_given_ = (data->player_party_[i].current_health_ > 0.0f ? (survivors > 0 ? data->experience_value_ / survivors : 0) : 0);
        new_member.xp_total_ = member_ptr->get_current_xp() + new_member.xp_given_;
        new_member.xp_bar_.set_main_fill(static_cast<float>(member_ptr->get_current_xp()) /
                                         static_cast<float>(member_ptr->get_xp_tnl()));
        new_member.xp_bar_.set_secondary_fill(static_cast<float>(member_ptr->get_current_xp()) /
                                              static_cast<float>(member_ptr->get_xp_tnl()));
        new_member.xp_bar_.set_secondary_fill(static_cast<float>(new_member.xp_total_) /
                                              static_cast<float>(member_ptr->get_xp_tnl()), 2.0f);
        members_.push_back(new_member);
    }
    visible_ = true;
    reset_position();
}
void post_battle::close_dialog()
{
    clean();
}
void post_battle::handle_input(const std::vector<sf::Event>& input_events)
{
    // TODO:: fix xp bar overflow
    double delta = state_manager::get()->get_delta();
    for (unsigned i = 0; i < members_.size(); ++i)
    {
        members_[i].hp_bar_.update(delta);
        members_[i].xp_bar_.update(delta);
        if (members_[i].xp_bar_.get_overflow() &&
            members_[i].xp_bar_.second_is_full())
        {
            members_[i].xp_bar_.reset_overflow();
            if (members_[i].displayed_level_ < 10)
            {
                members_[i].xp_bar_.set_main_fill(0.0f);
                members_[i].xp_bar_.set_secondary_fill(0.0f);
                members_[i].displayed_level_++;
                members_[i].xp_total_ -= members_[i].ptr_->get_tnl_to(members_[i].displayed_level_);

                members_[i].xp_bar_.set_secondary_fill(static_cast<float>(members_[i].xp_total_) /
                                                       static_cast<float>(members_[i].ptr_->get_tnl_to(members_[i].displayed_level_+1)), 2.0f);

                switch(members_[i].ptr_->get_class())
                {
                case encounter::hero_class::warrior:
                    members_[i].class_level_.setString("Level "+utils::to_str(members_[i].displayed_level_)+" Ironclad");
                    break;
                case encounter::hero_class::rogue:
                    members_[i].class_level_.setString("Level "+utils::to_str(members_[i].displayed_level_)+" Quicksilver");
                    break;
                case encounter::hero_class::mage:
                    members_[i].class_level_.setString("Level "+utils::to_str(members_[i].displayed_level_)+" Alchemist");
                    break;
                }
            }
        }
    }
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch(input_events[i].type)
        {
        case sf::Event::MouseMoved:
        {
            math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(input_events[i].mouseMove.x,
                                                                                          input_events[i].mouseMove.y));
            if (continue_text_.getGlobalBounds().contains(mouse_pos))
            {
                continue_text_.setColor(utils::rgb(0xFFFF00));
            }
            else
            {
                continue_text_.setColor(utils::rgb(0xFFFFFF));
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(input_events[i].mouseButton.x,
                                                                                              input_events[i].mouseButton.y));
            if (continue_text_.getGlobalBounds().contains(mouse_pos))
            {
                close_dialog();
            }
            break;
        }
        case sf::Event::KeyPressed:
            if (input_events[i].key.code == sf::Keyboard::Num1)
                close_dialog();
            break;
        default:break;
        }
    }
}
void post_battle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_)
        return;
    target.draw(background_, states);
    target.draw(title_, states);
    target.draw(continue_text_, states);
    for (unsigned i = 0; i < members_.size(); ++i)
    {
        target.draw(members_[i].name_, states);
        target.draw(members_[i].class_level_, states);
        target.draw(members_[i].kills_assists_, states);
        target.draw(members_[i].dmg_done_ff_, states);
        target.draw(members_[i].dmg_taken_, states);
        target.draw(members_[i].hp_bar_, states);
        target.draw(members_[i].xp_bar_, states);
    }
}
void post_battle::reset_position()
{
    pos_offset_ = state_manager::get()->get_dimensions();
    pos_offset_ = input_handler::get()->convert_mouse_pos((pos_offset_ - math::vector2f(background_.getLocalBounds().width, background_.getLocalBounds().height)) / 2.0f); // (1280 - 600)/2, (720 - 400)/2
    background_.setPosition(pos_offset_);
    title_.setPosition(pos_offset_ + math::vector2f(60.0f, 40.0f));
    continue_text_.setPosition(pos_offset_ + math::vector2f(60.0f, 400.0f));
    pos_offset_ += math::vector2f(60.0f, 90.0f);
    for (unsigned i = 0; i < members_.size(); ++i)
    {
        members_[i].name_.setPosition(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 0.0f + (i > 2 ? 160.0f : 0.0f)));
        members_[i].class_level_.setPosition(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 20.0f + (i > 2 ? 160.0f : 0.0f)));
        members_[i].kills_assists_.setPosition(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 60.0f + (i > 2 ? 160.0f : 0.0f)));
        members_[i].dmg_done_ff_.setPosition(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 75.0f + (i > 2 ? 160.0f : 0.0f)));
        members_[i].dmg_taken_.setPosition(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 90.0f + (i > 2 ? 160.0f : 0.0f)));
        members_[i].hp_bar_.set_position(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 40.0f + (i > 2 ? 160.0f : 0.0f)));
        members_[i].xp_bar_.set_position(pos_offset_ + math::vector2f((i > 2 ? i-3 : i)*220.0f, 50.0f + (i > 2 ? 160.0f : 0.0f)));
    }
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus

