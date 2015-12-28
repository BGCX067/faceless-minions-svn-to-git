#include <iostream>

#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/encounter/character_npc.hpp"
#include "icarus/utils/yth_handler.hpp"

namespace icarus
{
namespace encounter
{
state::state(sf::RenderWindow *const window) :
    base_state(window),
    background_(NULL),
    grid_(NULL),
    selected_character_(NULL),
    selection_indicator_(NULL),
    skill_handler_(NULL),
    skill_bar_(NULL),
    current_wave_(0),
    wave_count_(0),
    enemies_remaining_(0),
    enemies_total_(0),
    wave_timer_(3.0f)
{
    encounter::data* data(NULL);
    if ((data = reinterpret_cast<encounter::data*>(state_manager::get()->retrieve_data())))
    {
        setup_encounter(data);
    }
    // TODO:: ERROR HANDLING
}

state::~state()
{
    if (background_)
        delete background_;
    if (grid_)
        delete grid_;
    if (selection_indicator_)
        delete selection_indicator_;
    if (skill_handler_)
        delete skill_handler_;
    if (skill_bar_)
        delete skill_bar_;

    for (unsigned i = 0; i < player_list_.size(); ++i)
        if (player_list_[i])
            delete player_list_[i];

    for (unsigned i = 0; i < waves_.size(); ++i)
        for (unsigned k = 0; k < waves_[i].size(); ++k)
            if (waves_[i][k])
                delete waves_[i][k];
}

bool state::setup_encounter(encounter::data* new_encounter)
{
    current_encounter_ = new_encounter;
    utils::yth_node* scenario_root = current_encounter_->encounter_node_->
                              child(current_encounter_->current_scenario_);

    std::string path = "images/backgrounds/" + scenario_root->value() + ".png";
    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(path))
        background_ = new sf::Sprite(*bg_tex);

    grid_ = new grid();
    grid_->generate_grid(12, 27, 10.0f);
    skill_handler_ = new skill_handler(grid_);
    skill_bar_ = new ui::skillbar(this);

    selection_indicator_ = new sf::CircleShape(35.0f);
    selection_indicator_->setOrigin(selection_indicator_->getLocalBounds().width * 0.5f,
                                    selection_indicator_->getLocalBounds().height * 0.5f);
    selection_indicator_->setScale(1.0f, 0.66f);
    selection_indicator_->setFillColor(utils::argb(0x00000000));
    selection_indicator_->setOutlineColor(utils::argb(0xAFFFFF00));
    selection_indicator_->setOutlineThickness(3.0f);

    for (unsigned i = 0; i < current_encounter_->player_party_.size(); ++i)
    {
        player_list_.push_back(new character_hero(grid_, current_encounter_->player_party_[i].name_));
        character_hero* hero = reinterpret_cast<character_hero*>(player_list_.back());
        hero->setup_from_data(current_encounter_->player_party_[i], i);
        hero->set_cast_bar_enabled(true);
        if (!player_list_.back()->is_dead())
            place_character(player_list_.back(), current_encounter_->player_party_[i].initial_position_);
    }
    if (player_list_.size())
    {
        selected_character_ = player_list_[0];
        skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
    }

    for (unsigned i = 0; i < scenario_root->child_count("wave"); ++i)
    {
        std::vector<character_npc*> enemy_list;
        utils::yth_node* enemy_node = resource_handler::get()->get_root_node("enemies");
        for (unsigned k = 0; k < scenario_root->child("wave", i)->child_count("enemy"); ++k)
        {
            enemy_list.push_back(new character_npc(grid_));
            enemy_list.back()->setup_from_data(enemy_node->child(scenario_root->child("wave", i)->child("enemy", k)->value()));
            enemy_list.back()->set_health_bar_enabled(true);
            enemy_list.back()->set_cast_bar_enabled(true);

            math::vector2i enemy_pos;
            enemy_pos.x = atoi(scenario_root->child("wave", i)->child("enemy", k)->child("initial_x")->value().c_str());
            enemy_pos.y = atoi(scenario_root->child("wave", i)->child("enemy", k)->child("initial_y")->value().c_str());

            place_character(enemy_list.back(), enemy_pos);
        }
        waves_.push_back(enemy_list);
    }

    current_wave_ = 0;
    wave_count_ = scenario_root->child_count("wave");
    enemies_total_ = waves_[current_wave_].size();
    enemies_remaining_ = enemies_total_;
    current_encounter_->current_scenario_++;

    return true;
}

void state::pause_state()
{

}

void state::resume_state()
{

}

void state::handle_input(const std::vector<sf::Event>& key_events)
{
    for (unsigned i = 0; i < key_events.size(); ++i)
    {
        switch (key_events[i].type)
        {
        case sf::Event::KeyPressed:
            switch(key_events[i].key.code)
            {
            case sf::Keyboard::B:
                grid_->toggle_tile_display(!grid_->get_tile_display());
                break;
            case sf::Keyboard::Num1:
                if (player_list_.size() >= 1 && !player_list_[0]->is_dead())
                {
                    selected_character_ = player_list_[0];
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
                break;
            case sf::Keyboard::Num2:
                if (player_list_.size() >= 2 && !player_list_[1]->is_dead())
                {
                    selected_character_ = player_list_[1];
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
                break;
            case sf::Keyboard::Num3:
                if (player_list_.size() >= 3 && !player_list_[2]->is_dead())
                {
                    selected_character_ = player_list_[2];
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
                break;
            case sf::Keyboard::Num4:
                if (player_list_.size() >= 4 && !player_list_[3]->is_dead())
                {
                    selected_character_ = player_list_[3];
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
                break;
            case sf::Keyboard::Num5:
                if (player_list_.size() >= 5 && !player_list_[4]->is_dead())
                {
                    selected_character_ = player_list_[4];
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
                break;
            case sf::Keyboard::Num6:
                if (player_list_.size() >= 6 && !player_list_[5]->is_dead())
                {
                    selected_character_ = player_list_[5];
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
                break;
            default: break;
            }
            break;
        case sf::Event::KeyReleased:
            break;
        case sf::Event::TextEntered:
            break;
        case sf::Event::MouseButtonPressed:
            break;
        case sf::Event::MouseButtonReleased:
            if (key_events[i].mouseButton.button == sf::Mouse::Right)
            {
                if (active_cast_.active_)
                    active_cast_.active_ = false;
                grid_tile* selected = grid_->get_selected_tile();
                if (selected && selected_character_)
                    grid_->get_path_to_tile(selected->get_vector_pos(),
                                            selected_character_);
            }
            else if (key_events[i].mouseButton.button == sf::Mouse::Left)
            {
                if (active_cast_.active_)
                {
                    grid_tile* selected = grid_->get_selected_tile();
                    if (selected &&
                    (selected->get_range_from(active_cast_.caster_->get_vector_pos()) <=
                     active_cast_.caster_->get_skill(active_cast_.skill_).get_range()) &&
                     !active_cast_.caster_->is_dead())
                    {
                        cast_player_skill(active_cast_.caster_,
                                          active_cast_.skill_,
                                          selected);
                    }
                }
                else
                {
                    math::vector2i mouse_pos(input_handler::get()->get_mouse_xy());
                    character_base* new_selected = NULL;
                    for (unsigned i = 0; i < player_list_.size(); ++i)
                    {
                        if (player_list_[i]->get_bounds().contains(sf::Vector2f(mouse_pos.get_sf_vector()))
                            && (!new_selected ||
                                player_list_[i]->get_y_depth() > new_selected->get_y_depth()) &&
                            !player_list_[i]->is_dead())
                        {
                            new_selected = player_list_[i];
                        }
                    }
                    selected_character_ = (new_selected ? new_selected : selected_character_);
                    skill_bar_->set_selected_hero(dynamic_cast<character_hero*>(selected_character_));
                }
            }
            break;
        default: break;
        }
        skill_bar_->handle_input(key_events[i]);
    }
    if (!grid_->get_tile_display())
        grid_->handle_mouse_over(input_handler::get()->get_mouse_xy());
}

void state::handle_logic()
{
    double delta = state_manager::get()->get_delta();
    for (unsigned i = 0; i < player_list_.size(); ++i)
    {
        player_list_[i]->update_movement();
        player_list_[i]->update(delta);

        for (unsigned k = 0; k < 5;++k)
        {
            character_hero* hero = dynamic_cast<character_hero*>(player_list_[i]);
            if (hero->get_skill(k).get_autocast() &&
                hero->get_skill(k).is_ready() &&
                !hero->is_casting())
            {
                find_autocast_target(i, k);
            }
        }
        if (player_list_[i]->is_casting() && player_list_[i]->is_moving())
            skill_handler_->interrupt(player_list_[i]);
    }
    unsigned remaining = 0;
    for (unsigned i = 0; i < waves_[current_wave_].size(); ++i)
    {
        waves_[current_wave_][i]->update(delta);
        if (waves_[current_wave_][i]->is_casting() &&
            waves_[current_wave_][i]->is_moving())
            skill_handler_->interrupt(waves_[current_wave_][i]);
        if (!waves_[current_wave_][i]->is_dead())
            remaining++;
    }
    enemies_remaining_ = remaining;
    if (enemies_remaining_ == 0)
    {
        if (wave_count_ > current_wave_ + 1)
        {
            if (wave_timer_.update(delta))
            {
                current_wave_++;
                wave_timer_.reset();
            }
        }
        else if (current_encounter_->current_scenario_ < current_encounter_->scenario_count_)
        {
            if (wave_timer_.update(delta))
            {
                start_next_scenario();
                return;
            }
        }
        else
        {
            //exit_encounter();
            //return;
        }
    }
    cast_ai_skills();
    skill_handler_->update();
    if (selected_character_)
        selection_indicator_->setPosition(selected_character_->get_world_pos().get_sf_vector());
}

void state::draw() const
{
    if (background_)
        window_->draw(*background_);
    if (grid_)
        window_->draw(*grid_);
    if (selected_character_ && selection_indicator_)
        window_->draw(*selection_indicator_);
    for (unsigned i = 0; i < player_list_.size(); ++i)
    {
        player_list_[i]->update_anim();
        window_->draw(*player_list_[i]);
    }
    for (unsigned i = 0; i < waves_[current_wave_].size(); ++i)
    {
        waves_[current_wave_][i]->update_anim();
        window_->draw(*waves_[current_wave_][i]);
    }
    window_->draw(*skill_handler_);

    // UI
    if (skill_bar_)
        window_->draw(*skill_bar_);
}

void state::place_character(character_base* character,
                            const math::vector2i& vec_pos) const
{
    math::vector2f float_pos(grid_->get_tile_at(vec_pos)->get_world_pos());
    character->update_pos(float_pos, vec_pos);
    grid_->get_tile_at(character->get_target_vec_pos())->set_occupant(character);
}

void state::cast_player_skill(character_hero* active_char,
                              unsigned skill,
                              grid_tile* target_tile)
{
    if (target_tile == NULL && active_cast_.active_)
    {
        active_cast_.active_ = false;
    }
    else if (target_tile != NULL && active_cast_.active_)
    {
        skill_handler_->add_cast(active_char,
                                 &active_char->get_skill(skill),
                                 target_tile);
        active_cast_.active_ = false;
        state_manager::get()->set_cursor("default");
    }
    else
    {
        if (active_char->get_skill(skill).is_ready())
        {
            if (active_char->get_skill(skill).check_target_req())
            {
                active_cast_.caster_ = active_char;
                active_cast_.skill_ = skill;
                active_cast_.active_ = true;
                state_manager::get()->set_cursor("select");
            }
            else
            {
                skill_handler_->add_cast(active_char,
                                 &active_char->get_skill(skill),
                                 NULL);
            }
        }
        else
            std::cout << active_char->get_skill(skill).get_name() << " is on cooldown!" << std::endl;
    }
}

void state::cast_ai_skills()
{
    for (unsigned i = 0; i < waves_[current_wave_].size(); ++i)
    {
        composite_skill* skill;
        grid_tile* tile;
        character_npc* enemy = dynamic_cast<character_npc*>(waves_[current_wave_][i]);
        if ((skill = enemy->get_queued_skill()) &&
            (tile = enemy->get_targeted_tile()))
        {
            skill_handler_->add_cast(waves_[current_wave_][i], skill, tile);
        }
    }
}

void state::start_next_scenario()
{
    for (unsigned i = 0; i < player_list_.size(); ++i)
    {
        current_encounter_->player_party_[i].current_health_ = player_list_[i]->get_current_health();
    }

    state_manager::get()->change_state(new encounter::state(window_));
}

void state::exit_encounter()
{

}

void state::find_autocast_target(unsigned player_index, unsigned skill_index)
{
    std::vector<grid_tile*> tiles;
    character_hero* hero = dynamic_cast<character_hero*>(player_list_[player_index]);
    grid_->get_tiles_in_radius(hero->get_vector_pos(),
                               hero->get_skill(skill_index).get_range(),
                               tiles);
    for (unsigned i = 0; i < tiles.size(); ++i)
    {
        if (tiles[i]->get_occupant() != NULL &&
            tiles[i]->get_occupant()->is_ai())
        {
            skill_handler_->add_cast(hero,
                                 &hero->get_skill(skill_index),
                                 tiles[i]);
            return;
        }
    }
}
} // namespace encounter
} // namespace icarus
