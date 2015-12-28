#include "icarus/overworld/overworld_state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/sound.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/endgame/state.hpp"
#include <iostream>
#include <cstring>

namespace icarus
{
namespace overworld
{
overworld_state::overworld_state(sf::RenderWindow *window) :
        base_state(window),
        stats(statistics::get()),
        gui_(gui::gui_handler::get()),
        encounter_pointer(NULL),
        konami_index_(0),
        round_count_(0)
{
    konami_[0] = 73;
    konami_[1] = 73;
    konami_[2] = 74;
    konami_[3] = 74;
    konami_[4] = 71;
    konami_[5] = 72;
    konami_[6] = 71;
    konami_[7] = 72;
    konami_[8] = 1;
    konami_[9] = 0;
    setup_overworld();
}

overworld_state::~overworld_state()
{
    sound::get()->stop_bgm();
    sound::get()->unload_all();
    camera_.set_encounter_view(true);
    //dtor
}
bool overworld_state::setup_overworld()
{
    hex_sprite* player_spawn = NULL;
    if (state_manager::get()->is_new_game())
    {
        //ground
        grid_creator_.create_hex_grid(hex_grid_);
        grid_creator_.generate_starter_fractions(hex_grid_, nation_tiles_, stats->num_fractions_, 0.6);

        nations_.setup_nations(nations_information_, nation_tiles_[RED_],  RED_,   grid_creator_.get_overlay_sprire());
        nations_.setup_nations(nations_information_, nation_tiles_[GREEN_],GREEN_, grid_creator_.get_overlay_sprire());
        nations_.setup_nations(nations_information_, nation_tiles_[BLUE_], BLUE_,  grid_creator_.get_overlay_sprire());

        grid_creator_.generate_captial_cities(nation_tiles_[RED_], overworld::type_frac::RED_);
        grid_creator_.generate_captial_cities(nation_tiles_[GREEN_], overworld::type_frac::GREEN_);
        grid_creator_.generate_captial_cities(nation_tiles_[BLUE_], overworld::type_frac::BLUE_);
        grid_creator_.generate_captial_cities(nation_tiles_[SPAWN_], overworld::type_frac::SPAWN_);

        grid_creator_.generate_ground_ranges(hex_grid_, nature_tiles_,overworld::type_ground::ROCK_,24 + rand()%8 ,0.2,2,true);
        grid_creator_.generate_ground_ranges(hex_grid_, nature_tiles_,overworld::type_ground::FORREST_FLOOR_,30 + rand()%10 ,0.3,3,true);
        grid_creator_.generate_ground_ranges(hex_grid_, nature_tiles_,overworld::type_ground::WATTER_,20 + rand()%10 ,0.06,1,true);

        grid_creator_.variate_background_tiles(hex_grid_);
        //Nation creator
        grid_creator_.set_borders(hex_grid_);

        std::vector<unsigned> temp_ints;
        temp_ints.push_back(0);///MOUNTAIN_
        temp_ints.push_back(0);///FORREST_
        temp_ints.push_back(0);///HILL_
        temp_ints.push_back(15);///SETTLEMENT_
        temp_ints.push_back(40);///TAVERN_
        temp_ints.push_back(15);///CITY_
        temp_ints.push_back(10);///RUIN_
        temp_ints.push_back(15);///DUNGEON_
        temp_ints.push_back(4);///CASTLE_
        temp_ints.push_back(8);///FORTRESS_
        temp_ints.push_back(6);///TOWER_
        temp_ints.push_back(12);///OUTPOST_
        temp_ints.push_back(0);///CAPITAL_
        grid_creator_.setup_overlay(hex_grid_, temp_ints);

        player_spawn = grid_creator_.get_spawn();
        party_handler::get()->reset_party_gold();
    }
    else
    {
        /*for(unsigned i=0;i<3;i++)
            nations_information_.push_back(nation_info());*/
        state_manager::get()->load_game(this);
        player_spawn = &hex_grid_[stats->player_start_y][stats->player_start_x];
    }

    //player
    character_.setup_character(player_spawn,
                               resource_handler::get()->get_root_node("encounters"),
                               encounter_pointer,nations_, round_count_);
    // player party
    //party_handler::get()->add_party_member(1);
    //setup window

    sf::Vector2f temp = player_spawn->get_world_pos().get_sf_vector();
    camera_.setup(temp, window_);

    togle_fow = true;
    gui_->reset_position();

    utils::yth_node* audio_node = resource_handler::get()->get_root_node("audio");
    sound::get()->load_bgm(audio_node->child("overworld")->value(), true, true);
    sound::get()->load_sfx(audio_node->child("sfx")->child("coins")->value(), "coins");
    sound::get()->load_sfx(audio_node->child("sfx")->child("open_dialog")->value(), "open_dialog");
    sound::get()->load_sfx(audio_node->child("sfx")->child("close_dialog")->value(), "close_dialog");
    sound::get()->load_sfx(audio_node->child("sfx")->child("take_item")->value(), "take_item");
    sound::get()->load_sfx(audio_node->child("sfx")->child("drop_item")->value(), "drop_item");
    sound::get()->load_sfx(audio_node->child("sfx")->child("gust")->value(), "gust");

    return true;
}
void overworld_state::pause_state()
{
    sound::get()->stop_bgm();
    sound::get()->unload_all();
    camera_.set_encounter_view(true);
}

void overworld_state::save(std::ofstream& stream)
{
    uint16_t turn = character_.return_encounter()->get_turn_count();
    std::cout << "GAMETURN = " << turn << std::endl;
    stream.write(reinterpret_cast<char*>(&turn), sizeof(turn));
    uint8_t grid_width = stats->grid_width_;
    stream.write(reinterpret_cast<char*>(&grid_width), sizeof(grid_width));
    uint16_t tile_count = stats->grid_width_ * stats->grid_height_;
    stream.write(reinterpret_cast<char*>(&tile_count), sizeof(tile_count));
    for (unsigned i = 0; i < hex_grid_.size(); ++i)
    {
        for (unsigned k = 0; k < hex_grid_[i].size(); ++k)
        {
            utils::grid_tile tile;
            std::memset(tile.bytes, 0, sizeof(tile.bytes));
            tile.ground_ = unsigned(hex_grid_[i][k].get_nature());
            tile.overlay_ = unsigned(hex_grid_[i][k].get_overlay());
            tile.visible_ = unsigned(hex_grid_[i][k].get_occupation() ? 1 : 0);
            tile.battlezone_ = unsigned(0);

            if(hex_grid_[i][k].get_nation() > 2)
                tile.owner_ = unsigned(3);
            else
                tile.owner_ = unsigned(hex_grid_[i][k].get_nation());
            //if(hex_grid_[i][k].get_nation() < 3)
            if(hex_grid_[i][k].return_siege())
            {
                std::cout << "RETURN SIEGE = "<< hex_grid_[i][k].return_siege()<< std::endl;
                tile.battlezone_ = unsigned(1);
                tile.delay_ = unsigned(nations_information_[hex_grid_[i][k].get_attacking_nation()].get_delay(&hex_grid_[i][k]));
                tile.attacker_ = unsigned(hex_grid_[i][k].get_attacking_nation()+1);
                std::cout << " get_ attacker() " << hex_grid_[i][k].get_attacking_nation()+1 << std::endl;
            }
            else if(hex_grid_[i][k].get_overlay() == UNDER_CONSTRUCTION)
            {
                //std::cout << "under construction"
                tile.delay_ = unsigned(nations_information_[hex_grid_[i][k].get_nation()].get_delay(&hex_grid_[i][k]));
                tile.attacker_ = unsigned(0);
            }
            else
            {
                tile.delay_ = unsigned(0);
                tile.attacker_ = unsigned(0);
            }
            /*
            if(hex_grid_[i][k].return_siege())
            {
                tile.delay_ = unsigned(nations_information_[hex_grid_[i][k].get_nation()].get_delay(&hex_grid_[i][k]));
                tile.attacker_ = unsigned(nations_information_[hex_grid_[i][k].get_nation()].get_attacker());
            }
            else if(hex_grid_[i][k].get_overlay() == overworld::type_overlay::UNDER_CONSTRUCTION)
            {
                tile.delay_ = unsigned(nations_information_[hex_grid_[i][k].get_nation()].get_delay(&hex_grid_[i][k]));
                tile.attacker_ = unsigned(0);
            }
            else
            {
                tile.delay_ = unsigned(0);
                tile.attacker_ = unsigned(0);
            }
            */

            stream.write(tile.bytes, sizeof(tile.bytes));
        }
    }
    uint8_t party_x = character_.get_vec2i_position().x;
    stream.write(reinterpret_cast<char*>(&party_x), sizeof(party_x));
    uint8_t party_y = character_.get_vec2i_position().y;
    stream.write(reinterpret_cast<char*>(&party_y), sizeof(party_y));
    uint32_t party_gold = party_handler::get()->get_gold_amount();
    stream.write(reinterpret_cast<char*>(&party_gold), sizeof(party_gold));
    for (unsigned i = 0; i < nations_information_.size(); ++i)
    {
        uint8_t rep = int(nations_information_[i].get_reputation());
        stream.write(reinterpret_cast<char*>(&rep), sizeof(rep));
        uint16_t buildcoins = int(nations_information_[i].get_build_coins() * 100);
        stream.write(reinterpret_cast<char*>(&buildcoins), sizeof(buildcoins));
    }
    party_handler::get()->save(stream);
    stream.write(party_handler::get()->get_state().bytes, sizeof(party_handler::get()->get_state().bytes));
    for (unsigned i = 0; i < 6; ++i)
        stream.write(party_handler::get()->get_encounter_members()[i].bytes, sizeof(party_handler::get()->get_encounter_members()[i].bytes));
}
void overworld_state::load(std::ifstream& stream)
{
    uint16_t turn;
    stream.read(reinterpret_cast<char*>(&turn), sizeof(turn));
    round_count_ = turn;
    std::cout << "LOAD GAME TURN " << round_count_ << std::endl;
    uint8_t grid_width;
    stream.read(reinterpret_cast<char*>(&grid_width), sizeof(grid_width));
    uint16_t tile_count;
    stream.read(reinterpret_cast<char*>(&tile_count), sizeof(tile_count));

    stats->grid_width_ = unsigned(grid_width);
    stats->grid_height_ = static_cast<unsigned>(tile_count)/static_cast<unsigned>(grid_width);

    std::vector<utils::grid_tile> loaded_tiles;
    for (unsigned i = 0; i < unsigned(tile_count); ++i)
    {
        utils::grid_tile tile;
        std::memset(tile.bytes, 0, sizeof(tile.bytes));
        stream.read(tile.bytes, sizeof(tile.bytes));
        loaded_tiles.push_back(tile);
    }
    //std::cout << unsigned(tile_count) << " tiles found in save." << std::endl;
    uint8_t party_x;
    stream.read(reinterpret_cast<char*>(&party_x), sizeof(party_x));
    uint8_t party_y;
    stream.read(reinterpret_cast<char*>(&party_y), sizeof(party_y));
    std::cout << "Party location is: " << int(party_x) << "x, " << int(party_y) << "y. " << std::endl;
    stats->player_start_x = unsigned(party_x);
    stats->player_start_y = unsigned(party_y);

    std::cout << "DO THIS ONCE? "  << std::endl;
    for(unsigned i=0;i<3;i++)
    {
        std::vector <hex_sprite*> temp;
        nation_tiles_.push_back(temp);
    }
    nations_.setup_nations_from_save(nations_information_, nation_tiles_[RED_],  RED_,   grid_creator_.get_overlay_sprire());
    nations_.setup_nations_from_save(nations_information_, nation_tiles_[GREEN_],GREEN_, grid_creator_.get_overlay_sprire());
    nations_.setup_nations_from_save(nations_information_, nation_tiles_[BLUE_], BLUE_,  grid_creator_.get_overlay_sprire());
    grid_creator_.load_grid(loaded_tiles, hex_grid_, nations_information_, nation_tiles_);
    nations_.set_battlezones_from_save(hex_grid_, nations_information_, grid_creator_.get_battlezones_from_save());

    uint32_t party_gold;
    stream.read(reinterpret_cast<char*>(&party_gold), sizeof(party_gold));
    std::cout << "Party gold count is: " << int(party_gold) << std::endl;
    party_handler::get()->add_gold(unsigned(party_gold));

    //for (unsigned i = 0; i < nations_information_.size(); ++i)
    for (unsigned i = 0; i < 3; ++i)
    {
        uint8_t rep;
        stream.read(reinterpret_cast<char*>(&rep), sizeof(rep));
        uint16_t buildcoins;
        stream.read(reinterpret_cast<char*>(&buildcoins), sizeof(buildcoins));
        //std::cout << "Nation " << i << ": rep " << int(rep) << ", coins " << int(buildcoins)/100 << std::endl;
    }

    party_handler::get()->load(stream);
    stream.read(party_handler::get()->get_state().bytes, sizeof(party_handler::get()->get_state().bytes));

    for (unsigned i = 0; i < 6; ++i)
        stream.read(party_handler::get()->get_encounter_members()[i].bytes, sizeof(party_handler::get()->get_encounter_members()[i].bytes));

    gui_->load_dialog();
}

void overworld_state::resume_state()
{
    utils::yth_node* audio_node = resource_handler::get()->get_root_node("audio");
    sound::get()->load_bgm(audio_node->child("overworld")->value(), true, true);
    sound::get()->load_sfx(audio_node->child("sfx")->child("coins")->value(), "coins");
    sound::get()->load_sfx(audio_node->child("sfx")->child("open_dialog")->value(), "open_dialog");
    sound::get()->load_sfx(audio_node->child("sfx")->child("close_dialog")->value(), "close_dialog");
    sound::get()->load_sfx(audio_node->child("sfx")->child("take_item")->value(), "take_item");
    sound::get()->load_sfx(audio_node->child("sfx")->child("drop_item")->value(), "drop_item");
    sound::get()->load_sfx(audio_node->child("sfx")->child("gust")->value(), "gust");
    camera_.reset_view();
    encounter::data* data = dynamic_cast<encounter::data*>(state_manager::get()->retrieve_data());
    if (data != NULL)
    {
        if (data->current_scenario_ < data->scenario_count_-1)
        {
            data->current_scenario_++;
            party_handler::get()->get_state().state_ = unsigned(utils::encounter_state::encounter_in_progress);
            party_handler::get()->get_state().scenario_ = data->current_scenario_;
            party_handler::get()->get_state().party_xp_ = data->experience_value_;
            for (unsigned i = 0; i < data->player_party_.size(); ++i)
            {
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].active_ = 1;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].current_hp_ = data->player_party_[i].current_health_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].kills_ = data->player_party_[i].kills_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].assists_ = data->player_party_[i].assists_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].dmg_done_ = data->player_party_[i].damage_done_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].dmg_taken_ = data->player_party_[i].damage_taken_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].largest_given_ = data->player_party_[i].largest_given_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].largest_taken_ = data->player_party_[i].largest_taken_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].friendly_fire_ = data->player_party_[i].friendly_fire_;
            }
            state_manager::get()->save_game();
            state_manager::get()->push_state(states::encounter);
            return;
        }
        else
        {
            party_handler::get()->get_state().state_ = unsigned(utils::encounter_state::encounter_resolved);
            party_handler::get()->get_state().party_xp_ = data->experience_value_;
            for (unsigned i = 0; i < data->player_party_.size(); ++i)
            {
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].active_ = 1;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].current_hp_ = data->player_party_[i].current_health_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].kills_ = data->player_party_[i].kills_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].assists_ = data->player_party_[i].assists_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].dmg_done_ = data->player_party_[i].damage_done_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].dmg_taken_ = data->player_party_[i].damage_taken_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].largest_given_ = data->player_party_[i].largest_given_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].largest_taken_ = data->player_party_[i].largest_taken_;
                party_handler::get()->get_encounter_members()[data->player_party_[i].index_].friendly_fire_ = data->player_party_[i].friendly_fire_;
            }
            state_manager::get()->save_game();
            if(data->is_siege_battle)
            {
                if(character_.get_current_position()->get_overlay() == CAPITAL_)
                {
                    overworld::type_frac ruined_frac = character_.get_current_position()->get_nation();
                    //derp
                    grid_creator_.remove_structure(character_.get_current_position());
                    for(unsigned i=0;i<character_.get_current_position()->get_surrounding_tile_length();i++)
                        if(character_.get_current_position()->get_surrounding_tile(i)->get_overlay() == CAPITAL_)
                            grid_creator_.remove_structure(character_.get_current_position()->get_surrounding_tile(i));

                    while(nation_tiles_[ruined_frac].size()>0)
                    {
                        nation_tiles_[ruined_frac][0]->set_nation(TYPE_FRAC_NONE);
                        if(nation_tiles_[ruined_frac][0]->get_overlay() == overworld::type_overlay::FORTRESS_ ||
                           nation_tiles_[ruined_frac][0]->get_overlay() == overworld::type_overlay::OUTPOST_ ||
                           nation_tiles_[ruined_frac][0]->get_overlay() == overworld::type_overlay::CASTLE_ ||
                           nation_tiles_[ruined_frac][0]->get_overlay() == overworld::type_overlay::UNDER_CONSTRUCTION ||
                           nation_tiles_[ruined_frac][0]->get_overlay() == overworld::type_overlay::TOWER_  )
                            grid_creator_.remove_structure(nation_tiles_[ruined_frac][0]);
                        grid_creator_.set_single_border(nation_tiles_[ruined_frac][0], true);
                        nation_tiles_[ruined_frac].erase(nation_tiles_[ruined_frac].begin());
                    }
                    // remove all building ques,
                    nations_information_[ruined_frac].clear_buildin_queue();
                    // remove battlezone.
                    nations_information_[ruined_frac].deactivate_battle();
                    //update nation_info width information about death.
                    nations_information_[ruined_frac].set_loss();
                    if(nations_.is_game_over(nations_information_))
                        std::cout << "GAME HAS ENDED"<< std::endl;

                    std::cout << "FRACTION KILLED" << std::endl;
                }
                else
                    grid_creator_.remove_structure(character_.get_current_position());
            }
        }

        gui_->display_postbattle_dialog(data);

        if (!party_handler::get()->is_game_over())
        {
            party_handler::get()->add_gold(data->gold_reward_);
            if (data->gold_reward_ > 0)
                sound::get()->play_sfx("coins");
        }
    }
    camera_.center_view(character_.get_vec2f_position(type_vector2f::CURRENT));
    gui_->reset_position();
}
void overworld_state::handle_input(const std::vector<sf::Event>& key_events)
{
    //hello
    for (unsigned i = 0; i < key_events.size(); ++i)
    {
        switch (key_events[i].type)
        {
        case sf::Event::KeyPressed:
            switch (key_events[i].key.code)
            {
            case sf::Keyboard::F : togle_fow = !togle_fow; break;
            case sf::Keyboard::Space:
                camera_.center_view(character_.get_vec2f_position(type_vector2f::CURRENT));
                gui_->reset_position();
                break;
            default: break;
            }
            if (konami_index_ < unsigned(10) && unsigned(key_events[i].key.code) == konami_[konami_index_])
            {
                konami_index_++;
                if (konami_index_ == 10)
                    character_.enable_tiger();
            }
            else if (konami_index_ < 10)
            {
                konami_index_ = 0;
            }
            break;
        case sf::Event::KeyReleased:
            break;
        case sf::Event::TextEntered:
            break;
        case sf::Event::MouseButtonPressed:
            break;
        case sf::Event::MouseButtonReleased:
            if (key_events[i].mouseButton.button == sf::Mouse::Right &&
                gui_->mouse_enabled(input_handler::get()->convert_mouse_pos(math::vector2f(key_events[i].mouseButton.x, key_events[i].mouseButton.y))))
            {
                std::vector<hex_sprite*> click_targets;
                for(unsigned row = 0; row < stats->grid_height_; row++)
                {
                    for(unsigned col = 0; col < stats->grid_width_; col++)
                    {
                        math::vector2i temp_pos(key_events[i].mouseButton.x,key_events[i].mouseButton.y);
                        sf::Vector2f mouse_pos = window_->mapPixelToCoords(temp_pos.get_sf_vector());
                        if(grid_creator_.pass_bounds(hex_grid_ ,row, col).contains(mouse_pos))
                        {
                            click_targets.push_back(grid_creator_.return_sprite(hex_grid_ ,row, col));
                        }
                    }
                }
                if(click_targets.size() == 2)
                {
                    sf::FloatRect intersection;
                    if(click_targets[0]->return_bounds().intersects(click_targets[1]->return_bounds(),intersection))
                    {
                        int left = (click_targets[0]->get_world_pos().x <
                                    click_targets[1]->get_world_pos().x ? 0 : 1);
                        math::vector2f p1(intersection.left,
                                          intersection.top);
                        math::vector2f p2(intersection.left,
                                          intersection.top +
                                          intersection.height);
                        math::vector2f p3(intersection.left +
                                          intersection.width,
                                          intersection.top +
                                          (click_targets[left]->get_world_pos().y >
                                          click_targets[1-left]->get_world_pos().y ?
                                          intersection.height : 0));
                        int index = 0;
                        math::vector2f click_point(key_events[i].mouseButton.x,
                                         key_events[i].mouseButton.y);
                        if(math::point_in_triangle(p1, p2, p3, click_point))
                        {
                            index = left;
                        }
                        else
                        {
                            index = 1 - left;
                        }
                        hex_sprite* temp = click_targets[index];
                        click_targets.clear();
                        click_targets.push_back(temp);
                    }
                }
                // need to add checking in the target beeing close enought to reach,
                if(click_targets.size() == 1 && click_targets[0]->get_occupation() == overworld::state::VISITABLE_)
                    character_.move_to(click_targets[0]);
            }
            break;
        default: break;
        }
    }
    gui_->handle_input(key_events);
    camera_.update(key_events);
}
void overworld_state::handle_logic()
{
    //hello
    character_.update_character(nations_, nations_information_);
    //nations_.handle_nations(nations_information_);
    if (!gui_->dialog_visible() && party_handler::get()->is_game_over())
    {
        std::cout << "game over" << std::endl;
        endgame::data* data_ptr = new endgame::data();
        data_ptr->win_ = false;
        data_ptr->graveyard_ = party_handler::get()->get_graveyard();
        state_manager::get()->store_data(data_ptr);
        state_manager::get()->change_state(states::endgame);
    }
}
void overworld_state::draw() const
{
    for(unsigned row=0; row < hex_grid_.size(); row++)
    {
        for(unsigned col=0; col < hex_grid_[0].size();col++)
        {
            if(togle_fow)
            {
                if(grid_creator_.pass_state(hex_grid_, row, col) != overworld::state::UNREACHABLE_)
                    window_->draw(*grid_creator_.return_const_sprite(hex_grid_, row, col));
            }
            else
            {
                window_->draw(*grid_creator_.return_const_sprite(hex_grid_, row, col));
            }
        }
    }
    window_->draw(character_);
    window_->draw(*gui_);
}
} // namespace overworld
} // namespace icarus
