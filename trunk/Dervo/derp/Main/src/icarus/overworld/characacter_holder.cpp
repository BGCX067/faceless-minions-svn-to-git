#include "icarus/overworld/characacter_holder.hpp"
#include "icarus/utils/yth_handler.hpp"
namespace icarus
{
namespace overworld
{

characacter_holder::characacter_holder() :
    target_position_(0.0f, 0.0f),
    prev_position_(0.0f, 0.0f),
    vector_position_(0.0f, 0.0f),
    adjustment_position_(55, 100),
    move_speed_(40.0f),
    movement_step_(0.0f),
    placed_at(NULL)
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
                                         nation_creator& nations)
{
    current_position_ = spawn_point->get_sprite()->getPosition();
    target_position_ = spawn_point->get_sprite()->getPosition();
    prev_position_ = spawn_point->get_sprite()->getPosition();

    character_texture_.loadFromFile(stats.url_character_sprite_);
    character_sprite_.setTexture(character_texture_);
    character_sprite_.setTextureRect(sf::IntRect(0, 0,
                                                 stats.character_pixel_height_,
                                                 stats.character_pixel_width_));
    character_sprite_.setPosition(current_position_.get_sf_vector() +
                                  adjustment_position_.get_sf_vector());

    spawn_point->set_occupation(overworld::state::PLAYER_);
    spawn_point->set_surrounding_occupied(overworld::state::VISITABLE_);

    placed_at = spawn_point;

    //setup encounter
    encounter_list.setup_data_link(data, type_reference);
    encounter_list.setup_encounter(placed_at, nations);
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
    return vector_position_;
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
        current_position_=(math::v2lerp(prev_position_,
                                        target_position_,
                                        math::clamp(movement_step_,
                                                0.0f, 1.0f)));
        movement_step_ +=(math::clamp(move_speed_,0.0f, 1000.0f) * 0.001f);
        character_sprite_.setPosition((current_position_ +
                                       adjustment_position_).get_sf_vector());
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
        }
    }
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

}
const sf::Sprite* characacter_holder::return_sprite() const
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
