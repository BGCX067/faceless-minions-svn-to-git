#include "icarus/overworld/gui/encounter_dialog.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/encounter/data.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/utils.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
void encounter_dialog::clean()
{
    options_.clear();
    resolution_text_.clear();
    visible_ = false;
    resolved_ = false;
    continue_text_.setColor(utils::rgb(0xFFFFFF));
}
encounter_dialog::encounter_dialog()
:
    selected_option_(0),
    visible_(false),
    resolved_(false),
    experience_value_(0)
{
    utils::yth_node* ui_root = resource_handler::get()->get_root_node("ui");
    std::string bg_path = ui_root->child("encounter_dialog")->child("background")->value();
    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(bg_path))
        background_ = sf::Sprite(*bg_tex);

    //TODO:: remove magic numbers?
    title_.setCharacterSize(30);
    title_.setFont(*resource_handler::get()->get_font("text"));
    description_.setCharacterSize(20);
    description_.setFont(*resource_handler::get()->get_font("text"));

    continue_text_.setCharacterSize(20);
    continue_text_.setFont(*resource_handler::get()->get_font("text"));
    continue_text_.setString("1. Continue...");

    reset_position();
}
encounter_dialog::~encounter_dialog()
{
}

void encounter_dialog::reset_position()
{
    pos_offset_ = input_handler::get()->convert_mouse_pos(math::vector2f(340.0f, 160.0f)); // (1280 - 600)/2, (720 - 400)/2
    background_.setPosition(pos_offset_);
    title_.setPosition(pos_offset_ + math::vector2f(50.0f, 60.0f));
    description_.setPosition(pos_offset_ + math::vector2f(50.0f, 100.0f));
    continue_text_.setPosition(pos_offset_ + math::vector2f(50.0f, 300.0f));
    if (!resolved_)
    {
        for (unsigned i = 0; i < options_.size(); ++i)
            options_[i].option_text_.setPosition(pos_offset_ + math::vector2f(50.0f, 300.0f + i*30.0f));
    }
    else
    {
        resolution_text_[selected_option_].setPosition(pos_offset_ + math::vector2f(50.0f, 100.0f));
    }
}

void encounter_dialog::setup_dialog(utils::yth_node* encounter_node)
{
    reset_gold_expence();
    title_.setString(utils::wrap_string(encounter_node->child("display_name")->value(), 60));
    description_.setString(utils::wrap_string(encounter_node->child("display_text")->value(), 60));
    for (unsigned i = 0; i < encounter_node->child("options")->child_count("option"); ++i)
    {
        option new_option;
        new_option.option_text_.setCharacterSize(20);
        new_option.option_text_.setFont(*resource_handler::get()->get_font("text"));
        new_option.option_text_.setString(utils::wrap_string(utils::to_str(i+1) + ". " + encounter_node->child("options")->child("option", i)->child("display_name")->value(), 60));
        if (encounter_node->child("options")->child("option", i)->value() == "attack")
            new_option.result_ = encounter_resolution::combat;
        else if(encounter_node->child("options")->child("option", i)->value() == "stand_ground")
            new_option.result_ = encounter_resolution::combat_defence;
        else if(encounter_node->child("options")->child("option", i)->value() == "flank")
            new_option.result_ = encounter_resolution::combat_flank;
        else if(encounter_node->child("options")->child("option", i)->value() == "support_defending")
            new_option.result_ = encounter_resolution::aid_defending;
        else if(encounter_node->child("options")->child("option", i)->value() == "support_attacking")
            new_option.result_ = encounter_resolution::aid_attacking;
        else if(encounter_node->child("options")->child("option", i)->value() == "pay_healling")
        {
            new_option.result_ = encounter_resolution::pay_heal;
            gold_expences_[0] = atoi(encounter_node->child("options")->child("option", i)->child("required")->child("amount")->value().c_str());
        }
        else if(encounter_node->child("options")->child("option", i)->value() == "pay_member_random")
        {
            new_option.result_ = encounter_resolution::pay_member_random;
            gold_expences_[1] = atoi(encounter_node->child("options")->child("option", i)->child("required")->child("amount")->value().c_str());
        }
        else if(encounter_node->child("options")->child("option", i)->value() == "pay_member_rogue")
        {
            new_option.result_ = encounter_resolution::pay_member_rogue;
            gold_expences_[1] = atoi(encounter_node->child("options")->child("option", i)->child("required")->child("amount")->value().c_str());
        }
        else if(encounter_node->child("options")->child("option", i)->value() == "pay_member_warrior")
        {
            new_option.result_ = encounter_resolution::pay_member_warrior;
            gold_expences_[1] = atoi(encounter_node->child("options")->child("option", i)->child("required")->child("amount")->value().c_str());
        }
        else if(encounter_node->child("options")->child("option", i)->value() == "pay_member_mage")
        {
            new_option.result_ = encounter_resolution::pay_member_mage;
            gold_expences_[1] = atoi(encounter_node->child("options")->child("option", i)->child("required")->child("amount")->value().c_str());
        }
        else if(encounter_node->child("options")->child("option", i)->value() == "pay_item")
        {
            new_option.result_ = encounter_resolution::pay_item;
            gold_expences_[2] = atoi(encounter_node->child("options")->child("option", i)->child("required")->child("amount")->value().c_str());
        }
        else if(encounter_node->child("options")->child("option", i)->value() == "leave")
            new_option.result_ = encounter_resolution::peaceful;
        else
            new_option.result_ = encounter_resolution::peaceful;
        options_.push_back(new_option);
        sf::Text res_text;
        res_text.setCharacterSize(20);
        res_text.setFont(*resource_handler::get()->get_font("text"));
        res_text.setString(utils::wrap_string(encounter_node->child("options")->child("option", i)->child("resolution_text")->value(), 60));
        resolution_text_.push_back(res_text);
    }
    if (encounter_node->child_count("combat") > 0)
        combat_node_ = encounter_node->child("combat");
    visible_ = true;
    experience_value_ = atoi(encounter_node->child("xp_reward")->value().c_str());
    gold_income_ = atoi(encounter_node->child("gold_reward")->value().c_str());
    reset_position();
}
void encounter_dialog::select_option(unsigned selected)
{
    selected_option_ = selected;
    resolved_ = true;
    reset_position();
}
void encounter_dialog::close_dialog()
{
    switch(options_[selected_option_].result_)
    {
    case encounter_resolution::combat:
    {
        encounter::data* data_ptr = new encounter::data();
        data_ptr->encounter_node_ = combat_node_;
        data_ptr->current_scenario_ = 0;
        data_ptr->scenario_count_ = combat_node_->child_count("scenario");
        data_ptr->experience_value_ = experience_value_;
        party_handler::get()->get_party_data(data_ptr);
        state_manager::get()->store_data(data_ptr);
        state_manager::get()->push_state(states::encounter);
        break;
    }
    case encounter_resolution::combat_defence:
    {
        encounter::data* data_ptr = new encounter::data();
        data_ptr->encounter_node_ = combat_node_;
        data_ptr->current_scenario_ = 0;
        data_ptr->scenario_count_ = combat_node_->child_count("scenario");
        data_ptr->experience_value_ = experience_value_;
        party_handler::get()->get_party_data(data_ptr);
        state_manager::get()->store_data(data_ptr);
        state_manager::get()->push_state(states::encounter);

        // reduce stats of combat force
        break;
    }
    case encounter_resolution::combat_flank:
    {
        encounter::data* data_ptr = new encounter::data();
        data_ptr->encounter_node_ = combat_node_;
        data_ptr->current_scenario_ = 0;
        data_ptr->scenario_count_ = combat_node_->child_count("scenario");
        data_ptr->experience_value_ = experience_value_;
        party_handler::get()->get_party_data(data_ptr);
        state_manager::get()->store_data(data_ptr);
        state_manager::get()->push_state(states::encounter);

        // reduce stats of combat force
        break;
    }
    case encounter_resolution::aid_defending:
    {
        encounter::data* data_ptr = new encounter::data();
        data_ptr->encounter_node_ = combat_node_;
        data_ptr->current_scenario_ = 0;
        data_ptr->scenario_count_ = combat_node_->child_count("scenario");
        data_ptr->experience_value_ = experience_value_;
        party_handler::get()->get_party_data(data_ptr);
        state_manager::get()->store_data(data_ptr);
        state_manager::get()->push_state(states::encounter);

        // bonus reputation points towards the nation defending the tile
        // negative reputation points towards the nation attacking the tile

        //if winn, battlezone is broken defender won.
        break;
    }
    case encounter_resolution::aid_attacking:
    {
        encounter::data* data_ptr = new encounter::data();
        data_ptr->encounter_node_ = combat_node_;
        data_ptr->current_scenario_ = 0;
        data_ptr->scenario_count_ = combat_node_->child_count("scenario");
        data_ptr->experience_value_ = experience_value_;
        party_handler::get()->get_party_data(data_ptr);
        state_manager::get()->store_data(data_ptr);
        state_manager::get()->push_state(states::encounter);

        // bonus reputation points towards the nation attacking the tile
        // negative reputation points towards the nation defending the tile

        //if winn, battlezone is broken attacker took tile
        break;
    }
    case encounter_resolution::pay_heal:
    {
        // increase health
        if (party_handler::get()->get_gold_amount() >= gold_expences_[0])
        {
            party_handler::get()->heal_party(0.5f);
            party_handler::get()->subtract_gold(gold_expences_[0]);
        }
        break;
    }
    case encounter_resolution::pay_member_random:
    {
        // add new member
        if (party_handler::get()->get_gold_amount() >= gold_expences_[1])
        {
            party_handler::get()->add_party_member(0);
            party_handler::get()->subtract_gold(gold_expences_[1]);
        }
        break;
    }
    case encounter_resolution::pay_member_rogue:
    {
        // add new member
        if (party_handler::get()->get_gold_amount() >= gold_expences_[1])
        {
            party_handler::get()->add_party_member(encounter::hero_class::rogue, 0);
            party_handler::get()->subtract_gold(gold_expences_[1]);
        }
        break;
    }
    case encounter_resolution::pay_member_warrior:
    {
        // add new member
        if (party_handler::get()->get_gold_amount() >= gold_expences_[1])
        {
            party_handler::get()->add_party_member(encounter::hero_class::warrior, 0);
            party_handler::get()->subtract_gold(gold_expences_[1]);
        }
        break;
    }
    case encounter_resolution::pay_member_mage:
    {
        // add new member
        if (party_handler::get()->get_gold_amount() >= gold_expences_[1])
        {
            party_handler::get()->add_party_member(encounter::hero_class::mage, 0);
            party_handler::get()->subtract_gold(gold_expences_[1]);
        }
        break;
    }
    case encounter_resolution::pay_item:
    {
        // add new item,
        // out of order, until further notice.
        break;
    }
    case encounter_resolution::peaceful:
        break;
    default: break;
    }
    clean();
}
void encounter_dialog::handle_input(const std::vector<sf::Event>& input_events)
{
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch(input_events[i].type)
        {
        case sf::Event::MouseMoved:
        {
            math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(input_events[i].mouseMove.x,
                                                                                              input_events[i].mouseMove.y));
            if (!resolved_)
            {
                for (unsigned k = 0; k < options_.size(); ++k)
                {
                    if (options_[k].option_text_.getGlobalBounds().contains(mouse_pos))
                    {
                        options_[k].option_text_.setColor(utils::rgb(0xFFFF00));
                    }
                    else
                    {
                        options_[k].option_text_.setColor(utils::rgb(0xFFFFFF));
                    }
                }
            }
            else
            {
                if (continue_text_.getGlobalBounds().contains(mouse_pos))
                {
                    continue_text_.setColor(utils::rgb(0xFFFF00));
                }
                else
                {
                    continue_text_.setColor(utils::rgb(0xFFFFFF));
                }
            }
            break;
        }
        case sf::Event::MouseButtonReleased:
        {
            math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(input_events[i].mouseButton.x,
                                                                                              input_events[i].mouseButton.y));
            if (!resolved_)
            {
                for (unsigned k = 0; k < options_.size(); ++k)
                {
                    if (options_[k].option_text_.getGlobalBounds().contains(mouse_pos))
                    {
                        select_option(k);
                    }
                }
            }
            else
            {
                if (continue_text_.getGlobalBounds().contains(mouse_pos))
                {
                    close_dialog();
                }
            }
            break;
        }
        case sf::Event::KeyPressed:
            if (!resolved_)
            {
                switch (input_events[i].key.code)
                {
                case sf::Keyboard::Num1:
                    if (options_.size() >= 1) select_option(0); break;
                case sf::Keyboard::Num2:
                    if (options_.size() >= 2) select_option(1); break;
                case sf::Keyboard::Num3:
                    if (options_.size() >= 3) select_option(2); break;
                case sf::Keyboard::Num4:
                    if (options_.size() >= 4) select_option(3); break;
                case sf::Keyboard::Num5:
                    if (options_.size() >= 5) select_option(4); break;
                case sf::Keyboard::Num6:
                    if (options_.size() >= 6) select_option(5); break;
                case sf::Keyboard::Num7:
                    if (options_.size() >= 7) select_option(6); break;
                case sf::Keyboard::Num8:
                    if (options_.size() >= 8) select_option(7); break;
                case sf::Keyboard::Num9:
                    if (options_.size() >= 9) select_option(8); break;
                case sf::Keyboard::Num0:
                    if (options_.size() >= 0) select_option(9); break;
                default: break;
                }
            }
            else
            {
                if (input_events[i].key.code == sf::Keyboard::Num1)
                    close_dialog();
            }
            break;
        default:break;
        }
    }
}
void encounter_dialog::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_)
        return;
    target.draw(background_, states);
    target.draw(title_, states);
    if (!resolved_)
    {
        target.draw(description_, states);
        for (unsigned i = 0; i < options_.size(); ++i)
            target.draw(options_[i].option_text_, states);
    }
    else
    {
        target.draw(resolution_text_[selected_option_], states);
        target.draw(continue_text_, states);
    }
}
void encounter_dialog::reset_gold_expence()
{
    for(unsigned i=0;i<3;i++)
    {
        gold_expences_[i] = 0;
    }
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
