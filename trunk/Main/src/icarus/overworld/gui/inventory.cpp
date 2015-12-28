#include "icarus/overworld/gui/inventory.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/math/vector2.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/utils.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/overworld/gui/gui_handler.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/sound.hpp"

namespace icarus
{
namespace overworld
{
namespace gui
{
inventory::inventory()
:
    result_data_(NULL),
    visible_(false),
    loot_visible_(false),
    drag_enabled_(false)
{
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("inventory");
    std::string tex_path = dialog_root->child("background")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        background_ = sf::Sprite(*tex_ptr);
    tex_path = dialog_root->child("trash")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        trash_ = sf::Sprite(*tex_ptr);
    tex_path = dialog_root->child("loot")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        loot_background_ = sf::Sprite(*tex_ptr);
    tex_path = dialog_root->child("close_button")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        close_button_ = sf::Sprite(*tex_ptr);

    loot_[0] = NULL;
    loot_[1] = NULL;
    loot_[2] = NULL;
    loot_[3] = NULL;
    loot_[4] = NULL;
    loot_[5] = NULL;
    loot_icons_[0] = NULL;
    loot_icons_[1] = NULL;
    loot_icons_[2] = NULL;
    loot_icons_[3] = NULL;
    loot_icons_[4] = NULL;
    loot_icons_[5] = NULL;

    for (unsigned i = 0; i < 6; ++i)
    {
        tex_path = dialog_root->child("members")->child("background")->value();
        tex_ptr  = NULL;
        if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
            party_[i].background_ = sf::Sprite(*tex_ptr);
        tex_path = dialog_root->child("members")->child("deceased")->value();
        tex_ptr  = NULL;
        if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
            party_[i].deceased_ = sf::Sprite(*tex_ptr);
        party_[i].member_ = NULL;

        party_[i].name_.setFont(*resource_handler::get()->get_font("text"));
        party_[i].name_.setCharacterSize(14);
        party_[i].name_.setColor(utils::rgb(0xffdd4e));

        for (unsigned k = 0; k < 6; ++k)
            party_[i].inventory_icons_[k] = NULL;
    }
}
inventory::~inventory()
{
    for (unsigned i = 0; i < 6; ++i)
    {
        if (loot_[i] != NULL)
        {
            delete loot_[i];
            loot_[i] = NULL;
        }
        if (loot_icons_[i] != NULL)
        {
            delete loot_icons_[i];
            loot_icons_[i] = NULL;
        }
        for (unsigned k = 0; k < 6; ++k)
            if (party_[i].inventory_icons_[k] != NULL)
            {
                delete party_[i].inventory_icons_[k];
                party_[i].inventory_icons_[k] = NULL;
            }
    }
}

void inventory::setup_dialog(const std::vector<item*>& loot, encounter::data* result_data)
{
    sound::get()->play_sfx("open_dialog");
    result_data_ = result_data;
    utils::yth_node* icon_node = resource_handler::get()->get_root_node("ui")->child("item_icons");
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("inventory");
    math::vector2f origin = input_handler::get()->convert_mouse_pos(math::vector2f(0.0f, 0.0f));

    origin += math::vector2f(state_manager::get()->get_dimensions() - math::vector2f(background_.getLocalBounds().width, background_.getLocalBounds().height))/2;

    background_.setPosition(origin);
    math::vector2f offset(atof(dialog_root->child("close_button_x")->value().c_str()),
                          atof(dialog_root->child("close_button_y")->value().c_str()));
    close_button_.setPosition(origin + offset);
    math::vector2f loot_pos(atof(dialog_root->child("loot_x")->value().c_str()),
                          atof(dialog_root->child("loot_y")->value().c_str()));
    math::vector2f loot_1(atof(dialog_root->child("loot_x1")->value().c_str()),
                          atof(dialog_root->child("loot_y1")->value().c_str()));
    math::vector2f loot_2(atof(dialog_root->child("loot_x2")->value().c_str()),
                          atof(dialog_root->child("loot_y2")->value().c_str()));
    float loot_3_y(atof(dialog_root->child("loot_y3")->value().c_str()));

    loot_background_.setPosition(origin + loot_pos);
    loot_visible_ = false;

    offset = math::vector2f(atof(dialog_root->child("trash_x")->value().c_str()),
                            atof(dialog_root->child("trash_y")->value().c_str()));
    trash_.setPosition(origin + offset);

    for (unsigned i = 0; i < 6; ++i)
    {
        loot_icons_[i] = new icon(true);
        loot_icons_[i]->set_texture(*resource_handler::get()->get_texture(dialog_root->child("placeholder_icon")->value()));
        loot_icons_[i]->set_position(origin + math::vector2f(i%2 ? loot_2.x : loot_1.x, i>1 && i<4 ? loot_2.y : i < 2 ? loot_1.y : loot_3_y));
        loot_icons_[i]->set_visible(false);
    }
    for (unsigned i = 0; i < loot.size(); ++i)
    {
        loot_[i] = loot[i];
        loot_icons_[i]->set_texture(*resource_handler::get()->get_texture(icon_node->child(loot[i]->get_icon_name())->value()));
        configure_tooltip(loot_icons_[i], loot_[i]);
        loot_icons_[i]->set_visible(true);
        loot_visible_ = true;
    }
    offset = origin + math::vector2f(atof(dialog_root->child("members")->child("x_pos")->value().c_str()),
                                     atof(dialog_root->child("members")->child("y_pos")->value().c_str()));
    float spacing = atof(dialog_root->child("members")->child("spacing")->value().c_str());
    for (unsigned i = 0; i < 6; ++i)
    {
        party_[i].member_ = party_handler::get()->get_member(i);
        if (party_[i].member_ != NULL)
        {
            math::vector2f pos = offset + math::vector2f(i * spacing, 0.0f);
            party_[i].background_.setTexture(*resource_handler::get()->get_texture(dialog_root->child("members")->child("background")->value()));
            party_[i].background_.setPosition(pos);

            switch (party_[i].member_->get_class())
            {
                case encounter::hero_class::warrior: party_[i].portrait_.setTexture(*resource_handler::get()->get_texture(resource_handler::get()->get_root_node("classes")->child("warrior")->child("portrait_rot")->value())); break;
                case encounter::hero_class::rogue: party_[i].portrait_.setTexture(*resource_handler::get()->get_texture(resource_handler::get()->get_root_node("classes")->child("rogue")->child("portrait_rot")->value())); break;
                case encounter::hero_class::mage: party_[i].portrait_.setTexture(*resource_handler::get()->get_texture(resource_handler::get()->get_root_node("classes")->child("mage")->child("portrait_rot")->value())); break;
            }
            party_[i].portrait_.setPosition(pos);

            party_[i].name_.setString(party_[i].member_->get_name(14));
            party_[i].name_.setPosition(pos + math::vector2f(0.0f, atof(dialog_root->child("members")->child("name_y")->value().c_str())) +
                                       (math::vector2f(floor((party_[i].background_.getLocalBounds().width - party_[i].name_.getLocalBounds().width) / 2), 0.0f)));
            pos += math::vector2f(atof(dialog_root->child("members")->child("slots_x")->value().c_str()),
                                          atof(dialog_root->child("members")->child("slots_y")->value().c_str()));
            float i_spacing = atof(dialog_root->child("members")->child("slots_spacing")->value().c_str());
            for (unsigned k = 0; k < 6; ++k)
            {
                party_[i].inventory_icons_[k] = new icon(true);
                if (!party_[i].member_->get_item_at((k > 3 ? k-4 : k), (k > 3 ? true : false)))
                {
                    party_[i].inventory_icons_[k]->set_texture(*resource_handler::get()->get_texture(dialog_root->child("placeholder_icon")->value()));
                    party_[i].inventory_icons_[k]->set_visible(false);
                }
                else
                {
                    party_[i].inventory_icons_[k]->set_texture(*resource_handler::get()->get_texture(icon_node->child(party_[i].member_->get_item_at((k > 3 ? k-4 : k), (k > 3 ? true : false))->get_icon_name())->value()));
                    configure_tooltip(party_[i].inventory_icons_[k], party_[i].member_->get_item_at((k > 3 ? k-4 : k), (k > 3 ? true : false)));
                    party_[i].inventory_icons_[k]->set_visible(true);
                }
                party_[i].inventory_icons_[k]->set_position(pos + math::vector2f(0.0f, k * i_spacing));
            }
        }
    }
    visible_ = true;
}
void inventory::close_dialog()
{
    sound::get()->play_sfx("close_dialog");
    for (unsigned i = 0; i < 6; ++i)
        if (loot_[i] != NULL)
        {
            delete loot_[i];
            loot_[i] = NULL;
        }
    for (unsigned i = 0; i < 6; ++i)
        if (loot_icons_[i] != NULL)
        {
            delete loot_icons_[i];
            loot_icons_[i] = NULL;
        }
    for (unsigned i = 0; i < 6; ++i)
        for (unsigned k = 0; k < 6; ++k)
            if (party_[i].inventory_icons_[k] != NULL)
            {
                delete party_[i].inventory_icons_[k];
                party_[i].inventory_icons_[k] = NULL;
            }
    visible_ = false;

    if (result_data_ != NULL)
    {
        party_handler::get()->update_party(result_data_);
        party_handler::get()->get_state().state_ = unsigned(utils::encounter_state::no_encounter);
        state_manager::get()->save_game();
        result_data_ = NULL;
    }
    party_handler::get()->apply_items();
}
void inventory::handle_input(const std::vector<sf::Event>& input_events)
{
    if (!visible_)
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

                if (close_button_.getGlobalBounds().contains(mouse_pos))
                    gui_handler::get()->close_inventory();
                if (drag_enabled_ && dragged_item_.item_ != NULL)
                {
                    for (unsigned i = 0; i < 6; ++i)
                    {
                        if (loot_icons_[i] != NULL &&
                            !(dragged_item_.source_ == item_locations::loot
                              && i == dragged_item_.source_index_) &&
                            loot_icons_[i]->contains(mouse_pos))
                        {
                            switch (dragged_item_.source_)
                            {
                            case item_locations::loot:
                                {
                                    loot_[dragged_item_.source_index_] = loot_[i];
                                    loot_icons_[dragged_item_.source_index_] = loot_icons_[i];
                                    loot_[i] = dragged_item_.item_;
                                    loot_icons_[i] = dragged_item_.icon_;
                                    sound::get()->play_sfx("drop_item");
                                }
                                break;
                            case item_locations::backpack:
                                {
                                    item* temp = NULL;
                                    party_[dragged_item_.player_index_].member_->swap_item(loot_[i], temp, dragged_item_.source_index_ - 4, true);
                                    loot_[i] = temp;
                                    party_[dragged_item_.player_index_].inventory_icons_[dragged_item_.source_index_] = loot_icons_[i];
                                    loot_icons_[i] = dragged_item_.icon_;
                                    sound::get()->play_sfx("drop_item");
                                }
                                break;
                            case item_locations::inventory:
                                {
                                    item* temp = NULL;
                                    if (party_[dragged_item_.player_index_].member_->swap_item(loot_[i], temp, dragged_item_.source_index_, false))
                                    {
                                        loot_[i] = temp;
                                        party_[dragged_item_.player_index_].inventory_icons_[dragged_item_.source_index_] = loot_icons_[i];
                                        loot_icons_[i] = dragged_item_.icon_;
                                        sound::get()->play_sfx("drop_item");
                                    }
                                }
                                break;
                            }
                        }
                    }
                    for (unsigned i = 0; i < 6; ++i)
                    {
                        if (party_[i].member_ == NULL)
                            continue;
                        for (unsigned k = 0; k < 6; ++k)
                        {
                            if (party_[i].inventory_icons_[k] != NULL &&
                                !((dragged_item_.source_ == item_locations::backpack ||
                                   dragged_item_.source_ == item_locations::inventory) &&
                                  i == dragged_item_.player_index_ &&
                                  k == dragged_item_.source_index_) &&
                                party_[i].inventory_icons_[k]->contains(mouse_pos))
                            {
                                switch (dragged_item_.source_)
                                {
                                case item_locations::loot:
                                    {
                                        item* temp = NULL;
                                        if (party_[i].member_->swap_item(loot_[dragged_item_.source_index_], temp, (k > 3 ? k-4 : k), (k > 3 ? true : false)))
                                        {
                                            loot_[dragged_item_.source_index_] = temp;
                                            loot_icons_[dragged_item_.source_index_] = party_[i].inventory_icons_[k];
                                            party_[i].inventory_icons_[k] = dragged_item_.icon_;
                                            sound::get()->play_sfx("drop_item");
                                        }
                                    }
                                    break;
                                case item_locations::backpack:
                                    {
                                        if (k > 3 && i == dragged_item_.player_index_) break;
                                        if (dragged_item_.player_index_ == i)
                                        {
                                            if (!party_[i].member_->internal_swap(k, dragged_item_.source_index_ - 4))
                                                break;
                                            sound::get()->play_sfx("drop_item");
                                        }
                                        else
                                        {
                                            item* temp1 = NULL;
                                            if (!party_[i].member_->swap_item(dragged_item_.item_, temp1, (k > 3 ? k-4 : k), (k > 3 ? true : false))) break;
                                            item* temp2 = NULL;
                                            if (!party_[dragged_item_.player_index_].member_->swap_item(temp1, temp2, dragged_item_.source_index_-4, true)) break;
                                            sound::get()->play_sfx("drop_item");
                                        }

                                        party_[dragged_item_.player_index_].inventory_icons_[dragged_item_.source_index_] = party_[i].inventory_icons_[k];
                                        party_[i].inventory_icons_[k] = dragged_item_.icon_;
                                    }
                                    break;
                                case item_locations::inventory:
                                    {
                                        if (k < 4 && i == dragged_item_.player_index_) break;
                                        if (dragged_item_.player_index_ == i)
                                        {
                                            if (!party_[i].member_->internal_swap(dragged_item_.source_index_, k - 4))
                                                break;
                                            sound::get()->play_sfx("drop_item");
                                        }
                                        else
                                        {
                                            item* temp1 = NULL;
                                            if (!party_[i].member_->swap_item(dragged_item_.item_, temp1, (k > 3 ? k-4 : k), (k > 3 ? true : false))) break;
                                            item* temp2 = NULL;
                                            if (!party_[dragged_item_.player_index_].member_->swap_item(temp1, temp2, dragged_item_.source_index_, false)) break;
                                            sound::get()->play_sfx("drop_item");
                                        }
                                        party_[dragged_item_.player_index_].inventory_icons_[dragged_item_.source_index_] = party_[i].inventory_icons_[k];
                                        party_[i].inventory_icons_[k] = dragged_item_.icon_;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    if (trash_.getGlobalBounds().contains(mouse_pos))
                    {
                        switch (dragged_item_.source_)
                        {
                        case item_locations::loot:
                            delete loot_[dragged_item_.source_index_];
                            loot_[dragged_item_.source_index_] = NULL;
                            loot_icons_[dragged_item_.source_index_]->set_visible(false);
                            dragged_item_.icon_ = NULL;
                            dragged_item_.item_ = NULL;
                            break;
                        case item_locations::inventory:
                            party_[dragged_item_.player_index_].member_->destroy_item(dragged_item_.source_index_, false);
                            party_[dragged_item_.player_index_].inventory_icons_[dragged_item_.source_index_]->set_visible(false);
                            dragged_item_.icon_ = NULL;
                            dragged_item_.item_ = NULL;
                            break;
                        case item_locations::backpack:
                            party_[dragged_item_.player_index_].member_->destroy_item(dragged_item_.source_index_, true);
                            party_[dragged_item_.player_index_].inventory_icons_[dragged_item_.source_index_]->set_visible(false);
                            dragged_item_.icon_ = NULL;
                            dragged_item_.item_ = NULL;
                            break;
                        }
                        sound::get()->play_sfx("gust");
                    }
                }
                if (drag_enabled_)
                {
                    update_icon_positions();
                    drag_enabled_ = false;
                    dragged_item_.icon_ = NULL;
                    dragged_item_.item_ = NULL;
                }
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                math::vector2f mouse_pos(input_events[i].mouseButton.x,
                                         input_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);
                for (unsigned i = 0; i < 6; ++i)
                {
                    if (loot_icons_[i] != NULL &&
                        loot_icons_[i]->is_visible() &&
                        loot_icons_[i]->contains(mouse_pos))
                    {
                        drag_enabled_ = true;
                        dragged_item_.icon_ = loot_icons_[i];
                        dragged_item_.item_ = loot_[i];
                        dragged_item_.source_index_ = i;
                        dragged_item_.source_ = item_locations::loot;
                        sound::get()->play_sfx("take_item");
                        break;
                    }
                }
                if (drag_enabled_) break;
                for (unsigned i = 0; i < 6; ++i)
                {
                    if (party_[i].member_ == NULL) continue;
                    if (drag_enabled_) break;
                    for (unsigned k = 0; k < 6; ++k)
                    {
                        if (party_[i].inventory_icons_[k] != NULL &&
                            party_[i].inventory_icons_[k]->is_visible() &&
                            party_[i].inventory_icons_[k]->contains(mouse_pos))
                        {
                            drag_enabled_ = true;
                            dragged_item_.icon_ = party_[i].inventory_icons_[k];
                            dragged_item_.item_ = party_[i].member_->get_item_at((k > 3 ? k - 4 : k), (k > 3 ? true : false));
                            dragged_item_.source_index_ = k;
                            dragged_item_.player_index_ = i;
                            dragged_item_.source_ = (k > 3 ? item_locations::backpack : item_locations::inventory);
                            sound::get()->play_sfx("take_item");
                            break;
                        }
                    }
                }
                break;
            }
            case sf::Event::MouseMoved:
            {
                math::vector2f mouse_pos(input_events[i].mouseMove.x,
                                         input_events[i].mouseMove.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);

                if (drag_enabled_)
                {
                    dragged_item_.icon_->set_position(mouse_pos - (dragged_item_.icon_->get_dimensions() / 2));
                }

                break;
            }
        default: break;
        }
    }
}
void inventory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!visible_)
        return;
    target.draw(background_, states);
    target.draw(trash_, states);
    if (loot_visible_) target.draw(loot_background_, states);
    target.draw(close_button_, states);
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i].member_ != NULL)
        {
            target.draw(party_[i].background_, states);
            target.draw(party_[i].name_, states);
            target.draw(party_[i].portrait_, states);
        }
    }
    for (unsigned i = 0; i < 6; ++i)
    {
        if (i < 6)
        {
            if (loot_icons_[i] != NULL)
                target.draw(*loot_icons_[i], states);
        }
        if (party_[i].member_ != NULL)
        {
            for (unsigned k = 0; k < 6; ++k)
            {
                if (party_[i].inventory_icons_[k] != NULL)
                    target.draw(*party_[i].inventory_icons_[k], states);
            }
        }
    }
}

void inventory::configure_tooltip(icon* ic, item* it)
{
    ic->set_tt_enabled(true);
    ic->set_tt_data(it->get_tt_data_node());
}

void inventory::update_icon_positions()
{
    utils::yth_node* dialog_root = resource_handler::get()->get_root_node("ui")->child("inventory");
    math::vector2f origin = input_handler::get()->convert_mouse_pos(math::vector2f(0.0f, 0.0f));

    origin += math::vector2f(state_manager::get()->get_dimensions() - math::vector2f(background_.getLocalBounds().width, background_.getLocalBounds().height))/2;
    math::vector2f loot_1(atof(dialog_root->child("loot_x1")->value().c_str()),
                          atof(dialog_root->child("loot_y1")->value().c_str()));
    math::vector2f loot_2(atof(dialog_root->child("loot_x2")->value().c_str()),
                          atof(dialog_root->child("loot_y2")->value().c_str()));
    float loot_3_y(atof(dialog_root->child("loot_y3")->value().c_str()));
    for (unsigned i = 0; i < 6; ++i)
    {
        if (loot_icons_[i] != NULL)
            loot_icons_[i]->set_position(origin + math::vector2f(i%2 ? loot_2.x : loot_1.x, i>1 && i<4 ? loot_2.y : i < 2 ? loot_1.y : loot_3_y));
    }
    math::vector2f offset = origin + math::vector2f(atof(dialog_root->child("members")->child("x_pos")->value().c_str()),
                                     atof(dialog_root->child("members")->child("y_pos")->value().c_str()));
    float spacing = atof(dialog_root->child("members")->child("spacing")->value().c_str());
    for (unsigned i = 0; i < 6; ++i)
    {
        if (party_[i].member_ != NULL)
        {
            math::vector2f pos = offset + math::vector2f(i * spacing, 0.0f);
            pos += math::vector2f(atof(dialog_root->child("members")->child("slots_x")->value().c_str()),
                                          atof(dialog_root->child("members")->child("slots_y")->value().c_str()));
            float i_spacing = atof(dialog_root->child("members")->child("slots_spacing")->value().c_str());
            for (unsigned k = 0; k < 6; ++k)
            {
                party_[i].inventory_icons_[k]->set_position(pos + math::vector2f(0.0f, k * i_spacing));
            }
        }
    }
}
}   // namespace gui
}   // namespace overworld
}   // namespace icarus
