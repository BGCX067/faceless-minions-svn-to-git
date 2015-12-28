#include "icarus/overworld/characacter_holder.hpp"
#include "icarus/utils/yth_handler.hpp"
#include "icarus/overworld/party_handler.hpp"
namespace icarus
{
namespace overworld
{

characacter_holder::characacter_holder() :
    target_position_(0.0f, 0.0f),
    prev_position_(0.0f, 0.0f),
    vector_position_(0.0f, 0.0f),
    adjustment_position_(80.0f, 120.0f),
    move_speed_(2000.0f),
    movement_step_(0.0f),
    stats(statistics::get()),
    placed_at(NULL),
    tiger_(false)
{
    //ctor
}

characacter_holder::~characacter_holder()
{
    //dtor
}
void characacter_holder::setup_character(hex_sprite* spawn_point,
                                         utils::yth_node* data,
                                         utils::yth_node* type_reference,
                                         nation_creator& nations,
                                         unsigned tiles_moved)
{
    current_position_ = spawn_point->get_sprite()->getPosition();
    target_position_ = spawn_point->get_sprite()->getPosition();
    prev_position_ = spawn_point->get_sprite()->getPosition();

    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets")->child("overworld_character");
    character_sprite_.load_sprite(sprite_node->child("overworld_character"), sprite_node->child("path")->value());
    character_sprite_.set_sprite("one");
    character_sprite_.set_position(current_position_ + adjustment_position_);


    if (sf::Texture* bg_tex = resource_handler::get()->get_texture("images/overworld/character_marker.png"))
        player_indicator_ = sf::Sprite(*bg_tex);
    player_indicator_.setOrigin(round(player_indicator_.getLocalBounds().width/2),
                                round(player_indicator_.getLocalBounds().height/2));
    player_indicator_.setPosition(character_sprite_.get_position() - math::vector2f(0.0f, 50.0f));

    spawn_point->set_occupation(overworld::state::PLAYER_);
    spawn_point->set_surrounding_occupied(overworld::state::VISITABLE_);

    placed_at = spawn_point;

    //setup encounter
    encounter_list.setup_data_link(data, type_reference, tiles_moved);

    if (state_manager::get()->is_new_game())
        encounter_list.setup_encounter(placed_at, nations);
}

void characacter_holder::enable_tiger()
{
    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets")->child("tiger");
    character_sprite_.load_sprite(sprite_node->child("tiger"), sprite_node->child("path")->value());
    character_sprite_.set_sprite("horizontal");
    character_sprite_.set_position(current_position_ + adjustment_position_);
    tiger_ = true;
}

math::vector2f characacter_holder::get_vec2f_position(type_vector2f type)
{
    math::vector2i vector_position_;
    switch(type)
    {
    case CURRENT:   return current_position_;   break;
    case TARGET:    return target_position_;    break;
    case PREV:      return prev_position_;      break;
    }
    return math::vector2f(0.0f, 0.0f);
}
void characacter_holder::set_vec2f_position(type_vector2f type,
                                            math::vector2f new_value)
{
    switch(type)
    {
    case CURRENT:   current_position_ = new_value; break;
    case TARGET:    target_position_  = new_value; break;
    case PREV:      prev_position_    = new_value; break;
    }
}
math::vector2i  characacter_holder::get_vec2i_position()
{
    return placed_at->get_vector_position();
}
void characacter_holder::set_vec2i_position(math::vector2i new_value)
{
    vector_position_ = new_value;
}

void characacter_holder::update_character(nation_creator& nations,
                                          std::vector<nation_info>& n_info)
                                          // skal oppdateres hver frame
{
    //hvis current_pos != target_pos. Move
    // else stay
    if(target_position_ != current_position_)
    {

        character_sprite_.set_orientation((target_position_.x >= current_position_.x));
        if (tiger_)
        {
            if (math::compare(target_position_.y, current_position_.y))
            {
                character_sprite_.set_sprite("horizontal");
            }
            else if (target_position_.y > current_position_.y)
            {
                character_sprite_.set_sprite("downwards");
            }
            else
            {
                character_sprite_.set_sprite("upwards");
            }
        }

        current_position_=(math::v2lerp(prev_position_,
                                        target_position_,
                                        math::clamp(movement_step_,
                                                0.0f, 1.0f)));
        movement_step_ +=(math::clamp(float(move_speed_ * state_manager::get()->get_delta()),0.0f, 1000.0f) * 0.001f);
        character_sprite_.set_position(current_position_ + adjustment_position_);
    }
    else
    {
        //no need to move
            // do other stuff?

        // set prev pos to current.
        if(prev_position_ != current_position_)
        {
            nations.handle_nations(n_info);
            prev_position_ = current_position_;
            set_surrounding_tiles_to(VISITABLE_);
            encounter_list.setup_encounter(placed_at, nations);
            std::cout << "Ground " << placed_at->get_nature() << std::endl;
            std::cout << "Overlay " << placed_at->get_overlay() << std::endl;
            std::cout << "Nation " << placed_at->get_nation() << std::endl;
            //std::cout << "Nation " << placed_at->get_nation() << " has " << placed_at->return_presence() << " presence here. " << std::endl;
        }
    }
    if (!tiger_)
    {
        switch (party_handler::get()->get_party_count())
        {
        default:
        case 1:
            character_sprite_.set_sprite("one"); break;
        case 2:
            character_sprite_.set_sprite("two"); break;
        case 3:
            character_sprite_.set_sprite("three"); break;
        case 4:
            character_sprite_.set_sprite("four"); break;
        case 5:
            character_sprite_.set_sprite("five"); break;
        case 6:
            character_sprite_.set_sprite("six"); break;
        }
    }
    double x = double(state_manager::get()->get_milliseconds())/500.0;
    double offset_y = sin(x) * 25;
    double offset_x = sin(x/3) * 2;
    player_indicator_.setPosition(character_sprite_.get_position() - math::vector2f(offset_x, 70.0f + offset_y));
}
void characacter_holder::move_to(hex_sprite* target_point)//set target_position_
{
    if(target_position_ == current_position_)
    {
        runn_trough(target_point, VISIBLE_, 0, 1);
        movement_step_ = 0;
        placed_at = target_point;
        const sf::Sprite *temp_sprite = target_point->get_sprite();
        target_position_ = temp_sprite->getPosition();
    }
}
void characacter_holder::draw(sf::RenderTarget& target,
                              sf::RenderStates states) const
{
    target.draw(character_sprite_, states);
    target.draw(player_indicator_, states);
}
const ui::sprite_sheet* characacter_holder::return_sprite() const
{
    return &character_sprite_;
}
void characacter_holder::set_surrounding_tiles_to(overworld::state type)
{
    for(unsigned num_tiles = 0;
            num_tiles < placed_at->get_surrounding_tile_length();
            num_tiles++)
    {
        placed_at->get_surrounding_tile(num_tiles)->set_occupation(type);
    }
}
void characacter_holder::runn_trough(hex_sprite* target_tile,
                 overworld::state type,
                 unsigned this_size,
                 unsigned max_size)
{
    for(unsigned num_tiles =0;
            num_tiles < target_tile->get_surrounding_tile_length();
            num_tiles++)
    {
        target_tile->get_surrounding_tile(num_tiles)->set_occupation(type);
        if(this_size < max_size)
            runn_trough(target_tile->get_surrounding_tile(num_tiles),
                        type, this_size+1, max_size);
    }
}
encounter_calculator* characacter_holder::return_encounter()
{
    return &encounter_list;
}
} // namespace overworld
} // namespace icarus
