#include "icarus/overworld/overworld_state.hpp"
#include <iostream>

namespace icarus
{
namespace overworld
{
overworld_state::overworld_state(sf::RenderWindow *const window) :
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
    grid_creator_.create_hex_grid();
    grid_creator_.asign_nearby_tiles();
    grid_creator_.generate_starter_fractions(stats.num_fractions_, 0.3);
    grid_creator_.generate_nature_tiles(0,4,0.16,2,true);
    grid_creator_.variate_background_tiles();
    grid_creator_.generate_captial_cities(2, 0);
    grid_creator_.generate_captial_cities(4, 1);
    grid_creator_.generate_captial_cities(5, 2);
    grid_creator_.generate_captial_cities(9, 3);

    //player
    character_.setup_character(grid_creator_.get_spawn());

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
                        if(grid_creator_.pass_state(row, col) !=
                                overworld::state::UNREACHABLE_)
                        {
                            math::vector2i mouse_pos(key_events[i].mouseButton.x,
                                             key_events[i].mouseButton.y);
                            if(grid_creator_.pass_bounds(row, col).contains(
                                    sf::Vector2f(mouse_pos.get_sf_vector())))
                            {
                                click_targets.push_back(grid_creator_.
                                                    return_sprite(row, col));
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
}
void overworld_state::handle_logic()
{
    //hello
    character_.update_character();
}
void overworld_state::draw() const
{
    for(unsigned height=0; height < stats.grid_height_; height++)
    {
        for(unsigned width=0; width < stats.grid_width_;width++)
        {
            if(grid_creator_.pass_state(width, height) != overworld::state::UNREACHABLE_)
                window_->draw(*grid_creator_.return_const_sprite(width, height));
        }
    }
    window_->draw(*character_.return_sprite());
}
} // namespace overworld
} // namespace icarus
