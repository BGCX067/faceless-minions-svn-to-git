#include <iostream>
#include <cmath>

#include "icarus/encounter/character.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
void character::set_position(const math::vector2f& new_position)
{
    current_position_ = new_position;
    math::vector2f adjusted_position = current_position_ - anchor_offsets_;
    character_sprite_->setPosition(adjusted_position.get_sf_vector());
    debug_bounds_->setPosition(adjusted_position.get_sf_vector());
}

void character::add_surrounding_tiles(const math::vector2u& vec_pos,
                                      const grid* const grid_ptr,
                                      std::vector<path_node*>& node_vec)
{
    node_vec.clear();
    for (int row_offset = -1; row_offset < 2; ++row_offset)
    {
        for (int col_offset = -1; col_offset < 2; ++col_offset)
        {
            const grid_tile* const tmp_tile = grid_ptr->get_tile_at(
                                                        vec_pos.y + row_offset,
                                                        vec_pos.x + col_offset);
            if (!tmp_tile)
            {
                continue;
            }
            else if (tmp_tile->get_state() == tile_state::unoccupied)
            {
                path_node* tmp_node = new path_node(tmp_tile->get_vector_pos());
                if (row_offset != 0 && col_offset != 0)
                    tmp_node->g = 14;
                else
                    tmp_node->g = 10;
                node_vec.push_back(tmp_node);
            }
        }
    }
}

int character::compute_h(const math::vector2u& start_pos,
                         const math::vector2u& end_pos) const
{
    int h = abs(end_pos.x - start_pos.x) + abs(end_pos.y - start_pos.y);
    return h * 10;
}

character::character() :
    character_sprite_(NULL),
    debug_bounds_(NULL),
    debug_enabled_(false),
    anchor_offsets_(0.0f, 0.0f),
    current_position_(0.0f, 0.0f),
    target_position_(0.0f, 0.0f),
    prev_position_(0.0f, 0.0f),
    vector_position_(0, 0),
    move_speed_(100.0f),
    movement_step_(0.0f),
    sprite_width(63),
    sprite_height(75),
    sheet_width(756),
    sheet_height(250),
    sheet_column(0),
    sheet_row(0)
{
}

character::~character()
{
    if (character_sprite_)
        delete character_sprite_;
}

bool character::load_sprite(const std::string& texture_path,
                            const sf::IntRect& texture_coordinates,
                            const math::vector2f char_anchor)
{
    resource_handler::get_instance().load_texture(texture_path,
                                                  "encounter/character/test");
    character_sprite_ = new sf::Sprite(*(resource_handler::get_instance()
                                       .get_texture("encounter/character/test"))
                                       , texture_coordinates);
    anchor_offsets_ = char_anchor;
    // TODO:: use origin instead of anchor_offsets

    debug_bounds_ = new sf::RectangleShape();
    debug_bounds_->setSize(sf::Vector2f(character_sprite_->getLocalBounds().width,
                           character_sprite_->getLocalBounds().height));
    debug_bounds_->setFillColor(sf::Color(0, 0, 0, 0));
    debug_bounds_->setOutlineColor(sf::Color(255, 0, 0, 100));
    debug_bounds_->setOutlineThickness(1.0f);
    return true;
}

void character::draw(sf::RenderTarget& target,
                               sf::RenderStates states) const
{
    target.draw(*character_sprite_, states);
    if (debug_enabled_)
        target.draw(*debug_bounds_, states);
}

void character::animate()
{
    //character_sprite_->setOrigin((sprite_height*0.5), (sprite_width*0.5));
    character_sprite_->setTextureRect
    (sf::IntRect(sheet_column, sheet_row, sprite_width, sprite_height));

    sheet_column += sprite_width;
    if(sheet_column >= sheet_width)
    {
        sheet_column = 0;
    }
    if(sheet_row >= sheet_height)
    {
        sheet_row = 0;
    }
}

void character::move_to_tile(const math::vector2u& target_tile,
                             const grid* const grid_ptr)
{
    if (vector_position_ == target_tile)
        return;
    std::vector<path_node*> open_list, closed_list, surrounding;
    path_node* starting_node = new path_node(vector_position_);
    starting_node->g = 0;
    starting_node->h = compute_h(vector_position_, target_tile);
    starting_node->update_f();
    open_list.push_back(starting_node);

    path_node* current_node = NULL;
    bool target_found = false;
    while (!target_found)
    {
        current_node = NULL;
        int lowest_f_index;
        for (unsigned i = 0; i < open_list.size(); ++i)
        {
            if (!current_node || open_list[i]->f < current_node->f)
            {
                current_node = open_list[i];
                lowest_f_index = i;
            }
        }

        if (!current_node)
            break;
        open_list.erase(open_list.begin() + lowest_f_index);
        closed_list.push_back(current_node);

        if (current_node->vec_pos == target_tile)
        {
            target_found = true;
            break;
        }

        add_surrounding_tiles(current_node->vec_pos, grid_ptr, surrounding);
        for (int surr_i = 0; surr_i < static_cast<int>(surrounding.size());
             ++surr_i)
        {
            bool erased = false;
            for (unsigned clos_i = 0; clos_i < closed_list.size(); ++clos_i)
            {
                if (surrounding[surr_i]->vec_pos ==
                    closed_list[clos_i]->vec_pos)
                {
                    delete surrounding[surr_i];
                    surrounding.erase(surrounding.begin() + surr_i);
                    --surr_i;
                    erased = true;
                    break;
                }
            }
            if (erased)
                continue;
            for (unsigned open_i = 0; open_i < open_list.size(); ++open_i)
            {
                if (surrounding[surr_i]->vec_pos ==
                    open_list[open_i]->vec_pos)
                {
                    if (surrounding[surr_i]->g + current_node->g <
                        open_list[open_i]->g)
                    {
                        open_list[open_i]->parent = current_node;
                        open_list[open_i]->g = surrounding[surr_i]->g +
                                               current_node->g;
                        open_list[open_i]->update_f();
                    }

                    delete surrounding[surr_i];
                    surrounding.erase(surrounding.begin() + surr_i);
                    --surr_i;
                    erased = true;
                    break;
                }
            }
            if (erased)
                continue;

            surrounding[surr_i]->g += current_node->g;
            surrounding[surr_i]->h = compute_h(surrounding[surr_i]->vec_pos,
                                               target_tile);
            surrounding[surr_i]->update_f();
            surrounding[surr_i]->parent = current_node;
            open_list.push_back(surrounding[surr_i]);
            surrounding.erase(surrounding.begin() + surr_i);
            --surr_i;
        }
    }

    // TODO:: FIX MEMORY LEAKS!
    if (target_found)
    {
        for (unsigned i = 0; i < path_to_target_.size(); ++i)
            delete path_to_target_[i];
        path_to_target_.clear();

        path_node* node = current_node;
        while (node != NULL)
        {
            path_to_target_.push_back(node);
            node = node->parent;
        }

        for (unsigned i = 0; i < open_list.size(); ++i)
            delete open_list[i];
        for (unsigned i = 0; i < closed_list.size(); ++i)
        {
            bool is_in_path = false;
            for (unsigned k = 0; k < path_to_target_.size(); ++k)
            {
                if (closed_list[i] == path_to_target_[k])
                    is_in_path = true;
            }
            if (!is_in_path)
                delete closed_list[i];
        }
    }
    else
    {
        for (unsigned i = 0; i < open_list.size(); ++i)
            delete open_list[i];
        for (unsigned i = 0; i < closed_list.size(); ++i)
            delete closed_list[i];
        std::cout << "No available path found." << std::endl;
    }
}

void character::tp_to_tile(const math::vector2u& target_tile,
                           const grid* const grid_ptr)
{
    if (vector_position_ == target_tile)
        return;
    grid_tile* target = grid_ptr->get_tile_at(target_tile);
    target->set_occupant(this);
    set_position(target->get_char_anchor());
    vector_position_ = target_tile;
}

void character::update_movement(const grid* const grid_ptr)
{
    if (path_to_target_.size() == 0)
        return;
    grid_tile* target_tile = grid_ptr->get_tile_at(path_to_target_.back()->
                                                   vec_pos);
    if (target_position_ != target_tile->get_char_anchor())
    {
        target_position_ = target_tile->get_char_anchor();
        prev_position_ = current_position_;
    }
    if (current_position_ != target_position_)
    {
        set_position(math::v2lerp(prev_position_, target_position_,
                                  movement_step_));
        movement_step_ += (math::clamp(move_speed_, 0.0f, 1000.0f) * 0.001f);
        if (movement_step_ >= 0.5f)
        {
            if (target_tile->get_vector_pos() != vector_position_)
            {
                target_tile->set_occupant(this);
                grid_ptr->get_tile_at(vector_position_)->set_occupant(NULL);
                vector_position_ = target_tile->get_vector_pos();
            }
        }
    }
    else
    {
        // TODO:: MEMORY LEAKS!
        path_to_target_.pop_back();
        movement_step_ = 0.0f;
    }
}
} // namespace encounter
} // namespace icarus
