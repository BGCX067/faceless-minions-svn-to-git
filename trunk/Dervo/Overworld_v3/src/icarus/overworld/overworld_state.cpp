#include "icarus/overworld/overworld_state.hpp"
#include <iostream>

namespace icarus
{
namespace overworld
{
overworld_state::overworld_state(sf::RenderWindow *window) :
        base_state(window)
{
    //ctor

}

overworld_state::~overworld_state()
{
    //dtor
}
bool overworld_state::setup_overworld()
{
    //ground
    grid_creator_.create_hex_grid(hex_grid_);
    grid_creator_.asign_nearby_tiles(hex_grid_);

    grid_creator_.generate_starter_fractions(hex_grid_, nation_tiles_, stats.num_fractions_, 0.3);
    grid_creator_.generate_nature_tiles(hex_grid_, nature_tiles_,overworld::sheet_nature::ROCK_1,4,0.16,2,true);
    grid_creator_.variate_background_tiles(hex_grid_);
    grid_creator_.generate_captial_cities(nation_tiles_[RED_], overworld::nation::RED_, 0);
    grid_creator_.generate_captial_cities(nation_tiles_[GREEN_], overworld::nation::GREEN_, 1);
    grid_creator_.generate_captial_cities(nation_tiles_[BLUE_], overworld::nation::BLUE_, 2);
    grid_creator_.generate_captial_cities(nation_tiles_[SPAWN_], overworld::nation::SPAWN_, 3);

    //Nation creator
    nations_.get_nations_reference(nations_information_, nation_tiles_[overworld::nation::RED_],overworld::nation::RED_);
    nations_.get_nations_reference(nations_information_, nation_tiles_[overworld::nation::GREEN_],overworld::nation::GREEN_);
    nations_.get_nations_reference(nations_information_, nation_tiles_[overworld::nation::BLUE_],overworld::nation::BLUE_);
    grid_creator_.set_borders(hex_grid_);
    //player
    character_.setup_character(grid_creator_.get_spawn(), /* add yth_node here */);

    //setup window
    sf::Vector2f temp = grid_creator_.get_spawn()->get_world_pos().get_sf_vector();
    camera_.setup(temp, window_);

    return true;

}
void overworld_state::pause_state()
{

}

void overworld_state::resume_state()
{

}
void overworld_state::handle_input(const std::vector<sf::Event>& key_events)
{
    //hello
    for (unsigned i = 0; i < key_events.size(); ++i)
    {
        switch (key_events[i].type)
        {
        case sf::Event::KeyPressed:
            if (key_events[i].key.code == sf::Keyboard::B)
            {
                //grid_->toggle_tile_display(!grid_->get_tile_display());
            }
            break;
        case sf::Event::KeyReleased:
            break;
        case sf::Event::TextEntered:
            break;
        case sf::Event::MouseButtonPressed:
            break;
        case sf::Event::MouseButtonReleased:
            if (key_events[i].mouseButton.button == sf::Mouse::Left)
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
                            sf::Vector2f mouse_pos = window_->convertCoords(temp_pos.get_sf_vector());

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
    camera_.update(key_events);
}
void overworld_state::handle_logic()
{
    //hello
    character_.update_character(nations_, nations_information_);
    //nations_.handle_nations(nations_information_);
    /*
    for(unsigned col = 0; col < stats.grid_height_; col++)
    {
        for(unsigned row = 0; row < stats.grid_width_; row++)
        {
            unsigned temp = hex_grid_[col][row].get_nation()+6;
            sf::Vector2i sprite_type = grid_creator_.sprite_holder_[temp];
            hex_grid_[col][row].set_sprite_coordinates(&sprite_type,
                            overworld::type_texture::BASE_IMAGE);
        }
    }
    */
}
void overworld_state::draw() const
{
    for(unsigned height=0; height < stats.grid_height_; height++)
    {
        for(unsigned width=0; width < stats.grid_width_;width++)
        {
            if(grid_creator_.pass_state(hex_grid_, width, height) != overworld::state::UNREACHABLE_)
                window_->draw(*grid_creator_.return_const_sprite(hex_grid_, width, height));
        }
    }
    window_->draw(*character_.return_sprite());
}
} // namespace overworld
} // namespace icarus
