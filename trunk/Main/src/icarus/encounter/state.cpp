#include <iostream>

#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/encounter/character_npc.hpp"
#include "icarus/utils/yth_handler.hpp"
#include "icarus/sound.hpp"

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
    wave_timer_(3.0f),
    blackscreen_(utils::rgb(0), 0.5f, false),
    combat_ended_(false)
{
    encounter::data* data(NULL);
    if ((data = dynamic_cast<encounter::data*>(state_manager::get()->retrieve_data())))
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

    sound::get()->stop_bgm();
    sound::get()->unload_all();
}

bool state::setup_encounter(encounter::data* new_encounter)
{
    current_encounter_ = new_encounter;
    utils::yth_node* scenario_root = current_encounter_->encounter_node_->
                              child(current_encounter_->current_scenario_);
    utils::yth_node* scenario_node = resource_handler::get()->get_root_node("scenarios")->child(scenario_root->value());

    if (sf::Texture* bg_tex = resource_handler::get()->get_texture(scenario_node->child("background")->value()))
        background_ = new sf::Sprite(*bg_tex);
    if (!state_manager::get()->get_hd_status())
    {
        background_->setScale(0.66666f, 0.66666f);
    }

    grid_ = new grid();
    grid_->generate_grid(12, 27, 10.0f);
    skill_bar_ = new ui::skillbar(this);
    skill_handler_ = new skill_handler(grid_, skill_bar_);

    selection_indicator_ = new sf::CircleShape(utils::to_hd(30.0f));
    selection_indicator_->setOrigin(selection_indicator_->getLocalBounds().width * 0.5f,
                                    selection_indicator_->getLocalBounds().height * 0.5f);
    selection_indicator_->setScale(1.0f, 0.66f);
    selection_indicator_->setFillColor(utils::argb(0x00000000));
    selection_indicator_->setOutlineColor(utils::argb(0xAFFFFF00));
    selection_indicator_->setOutlineThickness(3.0f);

    for (unsigned i = 0; i < current_encounter_->player_party_.size(); ++i)
    {
        player_list_.push_back(new character_hero(grid_, current_encounter_->player_party_[i].name_));
        character_hero* hero = player_list_.back();
        hero->setup_from_data(current_encounter_->player_party_[i], i);
        hero->set_cast_bar_enabled(true);
        if (!player_list_.back()->is_dead())
        {
            untranslated_place(player_list_.back(), math::vector2i(2, (4+((i%6)%2?2*ceil((i%6)/2.0f):-2*ceil((i%6)/2.0f)))));
        }
    }
    if (player_list_.size())
    {
        set_selected_char(0);
    }

    for (unsigned i = 0; i < scenario_root->child_count("wave"); ++i)
    {
        std::vector<character_npc*> enemy_list;
        utils::yth_node* enemy_node = resource_handler::get()->get_root_node("enemies");
        for (unsigned k = 0; k < scenario_root->child("wave", i)->child_count(); ++k)
        {
            int enemy_count = ceil(atof(scenario_root->child("wave", i)->child(k)->value().c_str()) * current_encounter_->force_multiplier_);
            enemy_count = math::clamp(enemy_count, 1, 64);
            for (unsigned j = 0; j < unsigned(enemy_count); ++j)
            {
                enemy_list.push_back(new character_npc(grid_));
                enemy_list.back()->setup_from_data(enemy_node->child(scenario_root->child("wave", i)->child(k)->key()));
                enemy_list.back()->set_health_bar_enabled(true);
                enemy_list.back()->set_cast_bar_enabled(true);
                current_encounter_->experience_value_ += enemy_list.back()->get_xp_value();
            }
        }
        unsigned counter = 0;
        unsigned wave_count = ceil(float(enemy_list.size())/float(24));
        for (unsigned j = 0; j < wave_count; ++j)
        {
            std::vector<character_npc*> wave;
            for (unsigned k = 0; k < enemy_list.size()/wave_count; ++k)
            {
                if (counter < enemy_list.size())
                {
                    wave.push_back(enemy_list[counter]);
                    untranslated_place(enemy_list[counter], math::vector2i(26 - floor(k/6),
                                                                     (int(floor(k/6)) % 2 ? (4+((k%6)%2?2*ceil((k%6)/2.0f):-2*ceil((k%6)/2.0f))):
                                                                                            (5+((k%6)%2?2*ceil((k%6)/2.0f):-2*ceil((k%6)/2.0f))))));
                }
                counter++;
            }
            waves_.push_back(wave);
        }
    }

    // obstacles
    int max_obstacles = atoi(scenario_node->child("max_obstacles")->value().c_str());
    if (max_obstacles > 0)
    {
        unsigned obstacle_count = round(rand()%max_obstacles);
        for (unsigned i = 0; i < obstacle_count; ++i)
        {
            int obstacle_index = rand()%scenario_node->child_count("obstacle");
            obstacles_.push_back(new obstacle(grid_, scenario_node->child("obstacle", obstacle_index)->value()));
            int xmin = atoi(scenario_node->child("obs_lower_limit")->value().c_str());
            int xmax = atoi(scenario_node->child("obs_upper_limit")->value().c_str());
            untranslated_place(obstacles_.back(), math::vector2i(rand()%(xmax-xmin)+xmin,
                                                                 rand()%12));
        }
    }
    //end obstaclse

    utils::yth_node* audio = resource_handler::get()->get_root_node("audio");
    std::string bgm = "";
    if (audio->child_count(scenario_node->child("music")->value()) > 0)
    {
        bgm = audio->child(scenario_node->child("music")->value(), rand()%2)->value();
    }
    else
    {
        bgm = audio->child(scenario_node->child("music")->value())->value();
    }
    sound::get()->load_bgm(bgm, true, true);
    current_wave_ = 0;
    wave_count_ = waves_.size();
    enemies_total_ = waves_[current_wave_].size();
    enemies_remaining_ = enemies_total_;
    update_depth_sort();

    continue_text_.setString("This area is clear, would you like to continue or retreat?");
    continue_text_.setCharacterSize(22);
    continue_text_.setFont(*resource_handler::get()->get_font("text"));
    continue_text_.setPosition(math::vector2f((state_manager::get()->get_dimensions() - math::vector2f(continue_text_.getLocalBounds().width, continue_text_.getLocalBounds().height)) / 2).rounded());

    continue_yes_.setString("Yes, press on!");
    continue_yes_.setCharacterSize(20);
    continue_yes_.setFont(*resource_handler::get()->get_font("text"));
    math::vector2f pos((state_manager::get()->get_dimensions() - math::vector2f(continue_yes_.getLocalBounds().width, continue_yes_.getLocalBounds().height)) / 2 + math::vector2f(-200.0f, 50.0f));
    continue_yes_.setPosition(pos.rounded());

    continue_no_.setString("No, fall back!");
    continue_no_.setCharacterSize(20);
    continue_no_.setFont(*resource_handler::get()->get_font("text"));
    pos = math::vector2f((state_manager::get()->get_dimensions() - math::vector2f(continue_no_.getLocalBounds().width, continue_no_.getLocalBounds().height)) / 2 + math::vector2f(200.0f, 50.0f));
    continue_no_.setPosition(pos.rounded());

    blackscreen_.reset(true);

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
                if (!combat_ended_ && player_list_.size() >= 1 && !player_list_[0]->is_dead())
                {
                    set_selected_char(0);
                }
                else if (combat_ended_)
                {
                    start_next_scenario(true);
                }
                break;
            case sf::Keyboard::Num2:
                if (!combat_ended_ && player_list_.size() >= 2 && !player_list_[1]->is_dead())
                {
                    set_selected_char(1);
                }
                else if (combat_ended_)
                {
                    start_next_scenario(false);
                }
                break;
            case sf::Keyboard::Num3:
                if (player_list_.size() >= 3 && !player_list_[2]->is_dead())
                {
                    set_selected_char(2);
                }
                break;
            case sf::Keyboard::Num4:
                if (player_list_.size() >= 4 && !player_list_[3]->is_dead())
                {
                    set_selected_char(3);
                }
                break;
            case sf::Keyboard::Num5:
                if (player_list_.size() >= 5 && !player_list_[4]->is_dead())
                {
                    set_selected_char(4);
                }
                break;
            case sf::Keyboard::Num6:
                if (player_list_.size() >= 6 && !player_list_[5]->is_dead())
                {
                    set_selected_char(5);
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
                math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseButton.x,
                                                                                              key_events[i].mouseButton.y));
                if (active_cast_.active_)
                    active_cast_.active_ = false;
                grid_tile* selected = grid_->get_selected_tile();
                int index = -1;
                for (unsigned i = 0; i < waves_[current_wave_].size(); ++i)
                {
                    if (waves_[current_wave_][i]->contains(mouse_pos) &&
                        (index == -1 || waves_[current_wave_][i]->get_y_depth() > waves_[current_wave_][index]->get_y_depth()))
                    {
                        index = i;
                    }
                }
                if (index != -1)
                {
                    selected = NULL;
                    selected_character_->set_favored_target(waves_[current_wave_][index]);
                }
                else if (selected && selected_character_)
                {
                    if (selected->get_occupant() != NULL)
                    {
                        std::vector<grid_tile*> surrounding;
                        grid_->get_tiles_in_radius(selected->get_vector_pos(), 1, surrounding);
                        for (unsigned i = 0; i < surrounding.size(); ++i)
                        {
                            if (surrounding[i]->get_occupant() == NULL)
                                grid_->get_path_to_tile(surrounding[i]->get_vector_pos(),
                                                        selected_character_);
                        }
                    }
                    else
                    {
                        grid_->get_path_to_tile(selected->get_vector_pos(),
                                                selected_character_);
                    }
                }
                else if (selected_character_)
                {
                    selected_character_->set_favored_target(NULL);
                    selected_character_->set_queued_skill(-1, math::vector2i(0,0));
                }
            }
            else if (key_events[i].mouseButton.button == sf::Mouse::Left)
            {
                if (!combat_ended_ && active_cast_.active_)
                {
                    grid_tile* selected = grid_->get_selected_tile();
                    if (selected &&
                    (selected->get_range_from(active_cast_.caster_->get_vector_pos()) <=
                     active_cast_.caster_->get_skill(active_cast_.skill_).get_range()) &&
                     !active_cast_.caster_->is_dead())
                    {
                        if (active_cast_.caster_->is_moving())
                        {
                            active_cast_.caster_->clear_path();
                            active_cast_.caster_->set_queued_skill(active_cast_.skill_, selected->get_vector_pos());
                        }
                        else
                        {
                            cast_player_skill(active_cast_.caster_,
                                            active_cast_.skill_,
                                            selected);
                        }
                    }
                }
                else if (!combat_ended_)
                {
                    math::vector2f mouse_pos = input_handler::get()->get_mouse_xy<float>();
                    int new_selected = -1;
                    for (unsigned i = 0; i < player_list_.size(); ++i)
                    {
                        if (grid_->get_tile_at(player_list_[i]->get_vector_pos())->get_bounds().contains(mouse_pos)
                            && (new_selected == -1 ||
                                player_list_[i]->get_y_depth() > player_list_[new_selected]->get_y_depth()) &&
                            !player_list_[i]->is_dead())
                        {
                            new_selected = i;
                        }
                    }
                    if (new_selected == -1)
                    {
                        for (unsigned i = 0; i < player_list_.size(); ++i)
                        {
                            if (player_list_[i]->portrait_contains(mouse_pos))
                                new_selected = i;
                        }
                    }
                    if (new_selected != -1)
                        set_selected_char(new_selected);
                }
                else
                {
                    math::vector2f mouse_pos = input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseButton.x,
                                                                                              key_events[i].mouseButton.y));
                    if (continue_yes_.getGlobalBounds().contains(mouse_pos))
                    {
                        start_next_scenario(true);
                    }
                    else if (continue_no_.getGlobalBounds().contains(mouse_pos))
                    {
                        start_next_scenario(false);
                    }
                }
            }
            break;
        case sf::Event::MouseMoved:
        {
            if (!grid_->get_tile_display())
                grid_->handle_mouse_over(math::vector2i(key_events[i].mouseMove.x,
                                                        key_events[i].mouseMove.y));
            math::vector2f mouse_pos = input_handler::get()->get_mouse_xy<float>();
            int new_selected = -1;
            for (unsigned i = 0; i < player_list_.size(); ++i)
            {
                player_list_[i]->set_hover(false);
                if ((grid_->get_tile_at(player_list_[i]->get_vector_pos())->get_bounds().contains(mouse_pos) ||
                     player_list_[i]->contains(mouse_pos))
                    && (new_selected == -1 ||
                        player_list_[i]->get_y_depth() > player_list_[new_selected]->get_y_depth()) &&
                    !player_list_[i]->is_dead())
                {
                    new_selected = i;
                }
            }
            if (new_selected != -1)
                player_list_[new_selected]->set_hover(true);
            new_selected = -1;
            for (unsigned i = 0; i < waves_[current_wave_].size(); ++i)
            {
                waves_[current_wave_][i]->set_hover(false);
                if ((grid_->get_tile_at(waves_[current_wave_][i]->get_vector_pos())->get_bounds().contains(mouse_pos) ||
                     waves_[current_wave_][i]->contains(mouse_pos))
                    && (new_selected == -1 ||
                        waves_[current_wave_][i]->get_y_depth() > waves_[current_wave_][new_selected]->get_y_depth()) &&
                    !waves_[current_wave_][i]->is_dead())
                {
                    new_selected = i;
                }
            }
            if (new_selected != -1)
                waves_[current_wave_][new_selected]->set_hover(true);

            if (combat_ended_)
            {
                if (continue_yes_.getGlobalBounds().contains(mouse_pos))
                {
                    continue_yes_.setColor(utils::rgb(0xFFFF00));
                }
                else
                {
                    continue_yes_.setColor(utils::rgb(0xFFFFFF));
                }
                if (continue_no_.getGlobalBounds().contains(mouse_pos))
                {
                    continue_no_.setColor(utils::rgb(0xFFFF00));
                }
                else
                {
                    continue_no_.setColor(utils::rgb(0xFFFFFF));
                }
            }

            break;
        }
        default: break;
        }
        skill_bar_->handle_input(key_events[i]);
    }
}

void state::handle_logic()
{
    double profiler = state_manager::get()->get_game_time();
    double delta = state_manager::get()->get_delta();
    bool game_over = true;
    if (combat_ended_)
    {
        blackscreen_.update(delta);
    }
    for (unsigned i = 0; i < player_list_.size(); ++i)
    {
        player_list_[i]->update_movement();
        player_list_[i]->update(delta);
        if (!player_list_[i]->is_dead())
            game_over = false;
        for (unsigned k = 0; k < 5;++k)
        {
            character_hero* hero = player_list_[i];
            int index = hero->get_queued_skill().index_;
            if (index != -1 &&
                hero->get_skill(index).is_ready() &&
                !hero->is_casting() &&
                !hero->is_dead() &&
                !hero->is_incapacitated() &&
                !hero->is_moving() &&
                hero->get_target_path().size() == 0)
            {
                cast_player_skill(hero, index, grid_->get_tile_at(hero->get_queued_skill().target_));
                hero->set_queued_skill(-1, math::vector2i(0,0));
            }
            else if (hero->get_skill(k).get_autocast() &&
                hero->get_skill(k).is_ready() &&
                !hero->is_casting() &&
                !hero->is_dead() &&
                !hero->is_incapacitated())
            {
                find_autocast_target(i, k);
            }
            else if (hero->get_favored_target() == NULL &&
                     !hero->is_moving())
            {
                std::vector<grid_tile*> surrounding;
                grid_->get_tiles_in_radius(hero->get_vector_pos(), 2, surrounding);
                for (unsigned k = 0; k < surrounding.size(); ++k)
                    if (surrounding[k]->get_occupant() != NULL &&
                        surrounding[k]->get_occupant()->is_ai() &&
                        !surrounding[k]->get_occupant()->is_obstacle())
                        hero->set_favored_target(surrounding[k]->get_occupant());
            }
        }
        if (player_list_[i]->is_casting() && player_list_[i]->is_moving())
            skill_handler_->interrupt(player_list_[i]);
    }
    if (game_over)
    {
        if(wave_timer_.update(delta))
        {
            current_encounter_->gold_reward_ = 0;
            current_encounter_->loot_chance_ = 0;
            exit_encounter();
        }
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
                update_depth_sort();
            }
        }
        else if (current_encounter_->current_scenario_ +1 < current_encounter_->scenario_count_)
        {
            if (wave_timer_.update(delta))
            {
                combat_ended_ = true;
                return;
            }
        }
        else
        {
            if (wave_timer_.update(delta) && !combat_ended_)
            {
                exit_encounter();
                return;
            }
        }
    }
    for (unsigned i = 0; i < obstacles_.size(); ++i)
    {
        obstacles_[i]->update(delta);
    }
    cast_ai_skills();
    skill_handler_->update();
    skill_bar_->update();
    if (selected_character_)
        selection_indicator_->setPosition(selected_character_->get_world_pos().get_sf_vector());
    state_manager::get()->set_debug_profiler(state_manager::get()->get_game_time() - profiler);
    std::sort(depth_sorting_.begin(), depth_sorting_.end(), depth_sort());
}

void state::draw() const
{
    if (background_)
        window_->draw(*background_);
    if (grid_)
        window_->draw(*grid_);
    if (selected_character_ && selection_indicator_ &&
        !selected_character_->is_dead())
        window_->draw(*selection_indicator_);
    for (unsigned i = 0; i < depth_sorting_.size(); ++i)
    {
        depth_sorting_[i]->update_anim();
        window_->draw(*depth_sorting_[i]);
    }
    window_->draw(*skill_handler_);

    // UI
    if (skill_bar_)
        window_->draw(*skill_bar_);

    if (combat_ended_)
    {
        window_->draw(blackscreen_);
        window_->draw(continue_text_);
        window_->draw(continue_yes_);
        window_->draw(continue_no_);
    }
}

void state::place_character(character_base* character,
                            const math::vector2i& vec_pos) const
{
    math::vector2f float_pos(grid_->get_tile_at(vec_pos)->get_world_pos());
    character->update_pos(float_pos, vec_pos);
    grid_->get_tile_at(character->get_target_vec_pos())->set_occupant(character);
}

void state::untranslated_place(character_base* character,
                               const math::vector2i& vec_pos) const
{
    math::vector2f float_pos(grid_->get_tile_at(vec_pos, true)->get_world_pos());
    character->update_pos(float_pos, grid_->get_tile_at(vec_pos, true)->get_vector_pos());
    grid_->get_tile_at(character->get_target_vec_pos())->set_occupant(character);
}

void state::cast_player_skill(character_hero* active_char,
                              unsigned skill,
                              grid_tile* target_tile)
{
    if (target_tile == NULL && active_cast_.active_)
    {
        active_cast_.active_ = false;
        state_manager::get()->set_cursor("default");
    }
    else if (target_tile != NULL && active_cast_.active_)
    {
        if (!skill_handler_->add_cast(active_char,
                                 skill,
                                 &active_char->get_skill(skill),
                                 target_tile))
            return;
        active_cast_.active_ = false;
        state_manager::get()->set_cursor("default");
    }
    else if (active_char->get_skill(skill).is_available())
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
                if (active_char->is_moving())
                {
                    active_char->set_queued_skill(skill, active_char->get_vector_pos());
                    active_char->clear_path();
                }
                else
                {
                    skill_handler_->add_cast(active_char,
                                             skill,
                                             &active_char->get_skill(skill),
                                             grid_->get_tile_at(active_char->get_vector_pos()));
                }
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
        character_npc* enemy = waves_[current_wave_][i];
        if ((skill = enemy->get_queued_skill()) &&
            (tile = enemy->get_targeted_tile()) &&
            !enemy->is_incapacitated())
        {
            skill_handler_->add_cast(waves_[current_wave_][i], -1, skill, tile);
        }
    }
}

void state::start_next_scenario(bool progress)
{
    for (unsigned i = 0; i < player_list_.size(); ++i)
    {
        player_list_[i]->get_data(current_encounter_->player_party_[i]);
    }

    if (!progress)
    {
        current_encounter_->gold_reward_ = 0;
        current_encounter_->loot_chance_ = 0;
        current_encounter_->unique_chance_ = 0;
        current_encounter_->current_scenario_ = current_encounter_->scenario_count_;
    }
    state_manager::get()->pop_state();
}

void state::exit_encounter()
{
    for (unsigned i = 0; i < player_list_.size(); ++i)
    {
        player_list_[i]->get_data(current_encounter_->player_party_[i]);
    }
    current_encounter_->current_scenario_ = current_encounter_->scenario_count_;
    state_manager::get()->pop_state();
}

void state::find_autocast_target(unsigned player_index, unsigned skill_index)
{
    std::vector<grid_tile*> tiles;
    character_hero* hero = player_list_[player_index];
    unsigned range = hero->get_skill(skill_index).get_range();
    if (hero->get_favored_target() == NULL)
    {
        grid_->get_tiles_in_radius(hero->get_vector_pos(),
                                   range,
                                   tiles);
        grid_tile* skill_target = NULL;
        for (unsigned i = 0; i < tiles.size(); ++i)
        {
            if (tiles[i]->get_occupant() != NULL &&
                tiles[i]->get_occupant()->is_ai()&&
                !tiles[i]->get_occupant()->is_obstacle())
            {
                skill_target = tiles[i];
                math::vector2f a2(skill_target->get_world_pos());
                std::vector<grid_tile*> intersections;
                grid_->get_intersections(hero->get_world_pos(), a2, intersections);
                unsigned distance = 0;
                grid_tile* closest = NULL;
                for (unsigned i = 0; i < intersections.size(); ++i)
                {
                    if (distance == 0 ||
                        (intersections[i]->get_range_from(hero->get_vector_pos()) != 0 &&
                         intersections[i]->get_range_from(hero->get_vector_pos()) < distance))
                    {
                        distance = intersections[i]->get_range_from(hero->get_vector_pos());
                        closest = intersections[i];
                    }
                }
                if (closest != NULL &&
                    closest == skill_target)
                {
                    int index = (selected_character_ == hero ? skill_index : -1);
                    skill_handler_->add_cast(hero,
                                         index,
                                     &hero->get_skill(skill_index),
                                     skill_target);
                    return;
                }
            }
        }
    }
    else
    {
        grid_tile* target_tile = grid_->get_tile_at(hero->get_favored_target()->get_vector_pos());
        if (target_tile->get_range_from(hero->get_vector_pos()) <= range)
        {
            if (range > 1)
            {
                std::vector<grid_tile*> intersections;
                grid_->get_intersections(hero->get_world_pos(),
                                         target_tile->get_world_pos(),
                                         intersections);
                unsigned distance = 0;
                grid_tile* closest = NULL;
                for (unsigned i = 0; i < intersections.size(); ++i)
                {
                    if (distance == 0 ||
                        (intersections[i]->get_range_from(hero->get_vector_pos()) != 0 &&
                         intersections[i]->get_range_from(hero->get_vector_pos()) < distance))
                    {
                        distance = intersections[i]->get_range_from(hero->get_vector_pos());
                        closest = intersections[i];
                    }
                }
                if (closest != NULL &&
                    closest == target_tile)
                {
                    int index = (selected_character_ == hero ? skill_index : -1);
                    skill_handler_->add_cast(hero,
                                         index,
                                     &hero->get_skill(skill_index),
                                     target_tile);
                    return;
                }
            }
            else if (range == 1)
            {
                grid_->get_tiles_in_radius(hero->get_vector_pos(),
                                   range,
                                   tiles);
                for (unsigned i = 0; i < tiles.size(); ++i)
                {
                    if (tiles[i]->get_occupant() != NULL &&
                        tiles[i]->get_occupant() == hero->get_favored_target())
                    {
                        hero->clear_path();
                        int index = (selected_character_ == hero ? skill_index : -1);
                        skill_handler_->add_cast(hero,
                                             index,
                                         &hero->get_skill(skill_index),
                                         target_tile);
                        return;
                    }
                }
            }
        }
        else
        {
            grid_->get_tiles_in_radius(hero->get_favored_target()->get_vector_pos(),
                                   1,
                                    tiles);
            int closest = -1;
            for (unsigned i = 0; i < tiles.size(); ++i)
            {
                if (tiles[i]->get_occupant() == NULL &&
                    (closest == -1 || tiles[i]->get_range_from(hero->get_vector_pos()) <
                                      tiles[closest]->get_range_from(hero->get_vector_pos())))
                {
                    closest = i;
                }
            }
            if (closest != -1)
                grid_->get_path_to_tile(tiles[closest]->get_vector_pos(), hero);
        }
    }
}

void state::set_selected_char(unsigned index)
{
    if (selected_character_ == NULL ||
        selected_character_ != player_list_[index])
    {
        selected_character_ = player_list_[index];
        active_cast_.active_ = false;
        state_manager::get()->set_cursor("default");
        skill_bar_->set_selected_hero(selected_character_);
    }
    for (unsigned i = 0; i < player_list_.size(); ++i)
        player_list_[i]->set_selected((i == index));
}

void state::update_depth_sort()
{
    depth_sorting_.clear();
    for (unsigned i = 0; i < waves_[current_wave_].size(); ++i)
        depth_sorting_.push_back(waves_[current_wave_][i]);
    for (unsigned i = 0; i < player_list_.size(); ++i)
        depth_sorting_.push_back(player_list_[i]);
    for (unsigned i = 0; i < obstacles_.size(); ++i)
        depth_sorting_.push_back(obstacles_[i]);
}
} // namespace encounter
} // namespace icarus
