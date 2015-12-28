#include "icarus/overworld/overworld_state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/sound.hpp"
#include "icarus/overworld/party_handler.hpp"
#include "icarus/state_manager.hpp"
#include <iostream>

namespace icarus
{
namespace overworld
{
overworld_state::overworld_state(sf::RenderWindow *window) :
        base_state(window),
        gui_(gui::gui_handler::get())
{
    //ctor
    encounter_pointer = NULL;
    setup_overworld();
    togle_fow = true;
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
    //ground
    grid_creator_.create_hex_grid(hex_grid_);
    grid_creator_.generate_starter_fractions(hex_grid_, nation_tiles_, stats.num_fractions_, 0.3);

    nations_.get_nations_reference(nations_information_, nation_tiles_[overworld::type_frac::RED_],overworld::type_frac::RED_);
    nations_.get_nations_reference(nations_information_, nation_tiles_[overworld::type_frac::GREEN_],overworld::type_frac::GREEN_);
    nations_.get_nations_reference(nations_information_, nation_tiles_[overworld::type_frac::BLUE_],overworld::type_frac::BLUE_);


    grid_creator_.generate_captial_cities(nation_tiles_[RED_], overworld::type_frac::RED_);
    grid_creator_.generate_captial_cities(nation_tiles_[GREEN_], overworld::type_frac::GREEN_);
    grid_creator_.generate_captial_cities(nation_tiles_[BLUE_], overworld::type_frac::BLUE_);
    grid_creator_.generate_captial_cities(nation_tiles_[SPAWN_], overworld::type_frac::SPAWN_);

    grid_creator_.generate_ground_ranges(hex_grid_, nature_tiles_,overworld::type_ground::ROCK_,16 + rand()%8 ,0.09,2,true);
    grid_creator_.generate_ground_ranges(hex_grid_, nature_tiles_,overworld::type_ground::FORREST_FLOOR_,20 + rand()%10 ,0.13,3,true);

    //Nation creator
    grid_creator_.variate_background_tiles(hex_grid_);
    grid_creator_.set_borders(hex_grid_);

    std::vector<unsigned> temp_ints;
    temp_ints.push_back(0);//MOUNTAIN_
    temp_ints.push_back(0);//FORREST_
    temp_ints.push_back(0);//HILL_
    temp_ints.push_back(25);//SETTLEMENT_
    temp_ints.push_back(30);//TAVERN_
    temp_ints.push_back(10);//CITY_
    temp_ints.push_back(10);//RUIN_
    temp_ints.push_back(15);//DUNGEON_
    temp_ints.push_back(0);//CAPITAL_
    temp_ints.push_back(2);//CASTLE_
    temp_ints.push_back(6);//FORTRESS_
    temp_ints.push_back(4);//TOWER_
    temp_ints.push_back(8);//OUTPOST_
    grid_creator_.setup_overlay(hex_grid_, temp_ints);

    //player
    character_.setup_character(grid_creator_.get_spawn(),
                               resource_handler::get()->get_root_node("encounters"),
                               encounter_pointer,nations_);
    // player party
    party_handler::get()->add_party_member(0);

//    party_handler::get()->add_party_member(0);
//    party_handler::get()->add_party_member(0);
//    party_handler::get()->add_party_member(0);
//    party_handler::get()->add_party_member(0);
//    party_handler::get()->add_party_member(0);

    //setup window

    sf::Vector2f temp = grid_creator_.get_spawn()->get_world_pos().get_sf_vector();
    camera_.setup(temp, window_);

    gui_->reset_position();

    utils::yth_node* audio_node = resource_handler::get()->get_root_node("audio");
    sound::get()->load_bgm(audio_node->child("overworld")->value(), true, true);

    return true;
}
void overworld_state::pause_state()
{
    sound::get()->stop_bgm();
    sound::get()->unload_all();
    camera_.set_encounter_view(true);
}

void overworld_state::resume_state()
{
    utils::yth_node* audio_node = resource_handler::get()->get_root_node("audio");
    sound::get()->load_bgm(audio_node->child("overworld")->value(), true, true);
    camera_.set_encounter_view(false);
    encounter::data* data = dynamic_cast<encounter::data*>(state_manager::get()->retrieve_data());
    gui_->display_postbattle_dialog(data);
    party_handler::get()->update_party(data);
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
            if (key_events[i].mouseButton.button == sf::Mouse::Left &&
                !gui_->dialog_visible())
            {
                std::vector<hex_sprite*> click_targets;
                for(unsigned col = 0; col < stats.grid_height_; col++)
                {
                    for(unsigned row = 0; row < stats.grid_width_; row++)
                    {
                        if(grid_creator_.pass_state(hex_grid_ ,row, col) !=
                                overworld::state::UNREACHABLE_)
                        {
                            math::vector2i temp_pos(key_events[i].mouseButton.x,
                                             key_events[i].mouseButton.y);
                            sf::Vector2f mouse_pos = window_->mapPixelToCoords(temp_pos.get_sf_vector());

                            if(grid_creator_.pass_bounds(hex_grid_ ,row, col).contains(
                                    mouse_pos))
                            {
                                click_targets.push_back(grid_creator_.
                                                    return_sprite(hex_grid_ ,row, col));
                            }
                        }
                    }
                }
                if(click_targets.size() == 2)
                {
                    sf::FloatRect intersection;
                    if(click_targets[0]->return_bounds().intersects(
                                        click_targets[1]->return_bounds(),
                                        intersection))
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
        state_manager::get()->change_state(states::endgame);
    }
}
void overworld_state::draw() const
{
    for(unsigned height=0; height < stats.grid_height_; height++)
    {
        for(unsigned width=0; width < stats.grid_width_;width++)
        {
            if(togle_fow)
            {
                if(grid_creator_.pass_state(hex_grid_, width, height) != overworld::state::UNREACHABLE_)
                    window_->draw(*grid_creator_.return_const_sprite(hex_grid_, width, height));
            }
            else
            {
                window_->draw(*grid_creator_.return_const_sprite(hex_grid_, width, height));
            }
        }
    }
    window_->draw(*character_.return_sprite());
    window_->draw(*gui_);
}
} // namespace overworld
} // namespace icarus
