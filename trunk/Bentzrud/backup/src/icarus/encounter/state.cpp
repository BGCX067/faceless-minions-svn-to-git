#include <iostream>

#include "icarus/encounter/state.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/state_manager.hpp"
#include "icarus/encounter/character_npc.hpp"

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
    skill_handler_(NULL)
{
    encounter::data* data(NULL);
    if ((data = reinterpret_cast<encounter::data*>(state_manager::get_instance().retrieve_data())))
    {
        setup_encounter(*data);
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
}

bool state::setup_encounter(const encounter::data& new_encounter)
{
    current_encounter_ = new_encounter;
    std::string path;
    switch (current_encounter_.get_scenario())
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
    case scenario::undefined:
        break;
    }
    if (sf::Texture* bg_tex =
            resource_handler::get_instance().get_texture(path))
        background_ = new sf::Sprite(*bg_tex);

    grid_ = new grid();
    grid_->generate_grid(6, 15, 50.0f);
    skill_handler_ = new skill_handler(grid_);

    selection_indicator_ = new sf::CircleShape(35.0f);
    selection_indicator_->setOrigin(selection_indicator_->getLocalBounds().width * 0.5f,
                                    selection_indicator_->getLocalBounds().height * 0.5f);
    selection_indicator_->setScale(1.0f, 0.66f);
    selection_indicator_->setFillColor(sf::Color(0, 0, 0, 0));
    selection_indicator_->setOutlineColor(sf::Color(255, 255, 0, 175));
    selection_indicator_->setOutlineThickness(3.0f);

    //Them there characters for testing
    player_list.push_back(new character_hero(grid_, "TestChar 1", "images/characters/boxSheet.png"));
    place_character(player_list.back(), math::vector2i(0, 0));

    player_list.push_back(new character_hero(grid_, "TestChar 2", "images/characters/boxSheet.png"));
    player_list[1]->set_animation_state(icarus::anim_specs::walk);
    place_character(player_list.back(), math::vector2i(5, 5));


    // temp setup of enemy
    enemy_list.push_back(new character_npc(grid_));
    npc_data data;
    data.health_ = 100;
    data.move_speed_ = 100.0f;
    data.name_ = "enemy 1";

    skill_data skill;
    skill.name_ = "Enemy Incinerate";
    skill.cast_time_ = 1.0f;
    skill.cooldown_ = 5.0f;
    skill.range_ = 15;
    targeted_skill subskill;
    subskill.name = "Incinerate initial hit";
    subskill.damage = 10;
    subskill.type = skill_type::projectile;
    subskill.radius = 2;
    applied_effect effect;
    effect.name = "Incinerate burning damage";
    effect.damage = 5;
    effect.duration = 10.0f;
    effect.ticks = 5;
    effect.type = effect_type::burning;
    subskill.effect = effect;
    skill.subskills_.push_back(subskill);

    data.skills_[0] = skill;
    data.skill_count_ = 1;
    static_cast<character_npc*>(enemy_list.back())->setup_from_data(&data);
    place_character(enemy_list.back(), math::vector2i(3, 5));

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
            if (key_events[i].key.code == sf::Keyboard::B)
            {
                grid_->toggle_tile_display(!grid_->get_tile_display());
            }
            if (selected_character_ != NULL)
            {
                character_hero* hero_ptr = reinterpret_cast<character_hero*>(selected_character_);
                // TODO:: IMPLEMENT KEYBINDINGS
                if (key_events[i].key.code == sf::Keyboard::Q)
                {
                    if (active_cast_.active_)
                        active_cast_.active_ = false;
                    cast_player_skill(hero_ptr, 0);
                }
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
                     active_cast_.caster_->get_skill(active_cast_.skill_).get_range()))
                    {
                        cast_player_skill(active_cast_.caster_,
                                          active_cast_.skill_,
                                          selected);
                    }
                }
                else
                {
                    math::vector2i mouse_pos(key_events[i].mouseButton.x,
                                             key_events[i].mouseButton.y);
                    character_base* new_selected = NULL;
                    for (unsigned i = 0; i < player_list.size(); ++i)
                    {
                        if (player_list[i]->get_bounds().contains(sf::Vector2f(mouse_pos.get_sf_vector()))
                            && (!new_selected ||
                                player_list[i]->get_y_depth() > new_selected->get_y_depth()))
                        {
                            new_selected = player_list[i];
                        }
                    }
                    selected_character_ = new_selected;
                }
            }
            break;
        default: break;
        }
    }
    if (!grid_->get_tile_display())
        grid_->handle_mouse_over(input_handler::get_instance().get_mouse_xy());
}

void state::handle_logic()
{
    for (unsigned i = 0; i < player_list.size(); ++i)
    {
        player_list[i]->update_movement();
        player_list[i]->update(state_manager::get_instance().get_delta());
    }
    for (unsigned i = 0; i < enemy_list.size(); ++i)
    {
        enemy_list[i]->update(state_manager::get_instance().get_delta());
    }

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
    for (unsigned i = 0; i < player_list.size(); ++i)
    {
        player_list[i]->update_anim();
        window_->draw(*player_list[i]);
    }
    for (unsigned i = 0; i < enemy_list.size(); ++i)
    {
        enemy_list[i]->update_anim();
        window_->draw(*enemy_list[i]);
    }
    window_->draw(*skill_handler_);
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
    if (target_tile != NULL && active_cast_.active_)
    {
        skill_handler_->add_cast(active_char,
                                 &active_char->get_skill(skill),
                                 target_tile);
        active_cast_.active_ = false;
    }
    else
    {
        if (active_char->get_skill(0).is_ready())
        {
            if (active_char->get_skill(0).check_target_req())
            {
                active_cast_.caster_ = active_char;
                active_cast_.skill_ = skill;
                active_cast_.active_ = true;
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
} // namespace encounter
} // namespace icarus
