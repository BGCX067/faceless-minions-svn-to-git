#include <iostream>

#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
state::state(sf::RenderWindow *const window) :
    window_(window),
    background_(NULL),
    grid_(NULL),
    selected_character_(NULL),
    selection_indicator_(NULL)
{
}

state::~state()
{
    if (background_)
        delete background_;
    if (grid_)
        delete grid_;
    if (selection_indicator_)
        delete selection_indicator_;
}

bool state::setup_encounter(const encounter_data& new_encounter)
{
    current_encounter_ = new_encounter;
    std::string path;
    switch (current_encounter_.active_scenario)
    {
    case scenario::forest:
        path = "images/backgrounds/forest.png";
        break;
    case scenario::mountain:
        path = "images/backgrounds/mountain.png";
        break;
    case scenario::coast:
        path = "images/backgrounds/coast.png";
        break;
    case scenario::hills:
        path = "images/backgrounds/hills.png";
        break;
    }
    resource_handler::get_instance().load_texture(path, "combat/background");

    sf::Texture* bg_tex =
            resource_handler::get_instance().get_texture("combat/background");
    background_ = new sf::Sprite(*bg_tex);

    grid_ = new grid();
    grid_->generate_grid(6, 15, 75.0f, 50.0f);

    selection_indicator_ = new sf::CircleShape(35.0f);
    selection_indicator_->setOrigin(selection_indicator_->getLocalBounds().width * 0.5f,
                                    selection_indicator_->getLocalBounds().height * 0.5f);
    selection_indicator_->setScale(1.0f, 0.66f);
    selection_indicator_->setFillColor(sf::Color(0, 0, 0, 0));
    selection_indicator_->setOutlineColor(sf::Color(255, 255, 0, 175));
    selection_indicator_->setOutlineThickness(3.0f);

    // temp for testing pathfinding
    player_list.push_back(new character());
    player_list.back()->load_sprite("images/characters/animation_mal_walk.png",
                                    sf::IntRect(sf::Vector2i(0, 0),
                                                sf::Vector2i(63, 140)),
                                    math::vector2f(30.0f, 130.0f));
    player_list.back()->tp_to_tile(math::vector2u(5, 1), grid_);
    player_list.back()->set_debug(true);

    window_->setFramerateLimit(24);

    return true;
}

void state::handle_input()
{
    if (input_handler::get_instance().get_key_down(sf::Keyboard::Key::B))
    {
        grid_->toggle_tile_display(true);
    }
    else
    {
        grid_->toggle_tile_display(false);
        grid_->handle_mouse_over(input_handler::get_instance().get_mouse_xy());
    }
    if (input_handler::get_instance().get_mouse_button(sf::Mouse::Right))
    {
        grid_tile* selected = grid_->get_selected_tile();
        if (selected && selected_character_)
            selected_character_->move_to_tile(selected->get_vector_pos(),
                                              grid_);
    }
    else if (input_handler::get_instance().get_mouse_button(sf::Mouse::Left))
    {
        math::vector2i mouse_pos = input_handler::get_instance().get_mouse_xy();
        character* new_selected = NULL;
        for (unsigned i = 0; i < player_list.size(); ++i)
        {
            if (player_list[i]->get_bounds().contains(sf::Vector2f(mouse_pos.get_sf_vector()))
                && (!new_selected ||
                    player_list[i]->get_z_depth() > new_selected->get_z_depth()))
            {
                new_selected = player_list[i];
            }
        }
        selected_character_ = new_selected;
    }
}

void state::handle_logic()
{
    for (unsigned i = 0; i < player_list.size(); ++i)
        player_list[i]->update_movement(grid_);
    if (selected_character_)
        selection_indicator_->setPosition(selected_character_->get_position().get_sf_vector());
}

void state::draw() const
{
    if (background_)
        window_->draw(*background_);
    if (grid_)
        window_->draw(*grid_);
    if (selected_character_ && selection_indicator_)
        window_->draw(*selection_indicator_);
    for (unsigned i = 0; i < player_list.size(); ++i)
    {
        window_->draw(*player_list[i]);
        player_list[i]->animate();
    }
}
} // namespace encounter
} // namespace icarus
