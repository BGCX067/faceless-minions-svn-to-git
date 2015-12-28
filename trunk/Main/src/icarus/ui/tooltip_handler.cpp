#include "icarus/ui/tooltip_handler.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/utils.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/state_manager.hpp"

namespace icarus
{
namespace ui
{
tooltip_handler* tooltip_handler::instance_ = NULL;
tooltip_handler::tooltip_handler()
:
    frame_(sf::Vector2f(40.0f, 40.0f)),
    text_spacing_(4.0f)
{
    frame_.setFillColor(utils::argb(0xDD333333));
    frame_.setOutlineColor(utils::rgb(0xBBBBBB));
    frame_.setOutlineThickness(3.0f);
}
tooltip_handler::~tooltip_handler()
{
}

void tooltip_handler::update_tooltip(const sf::Event& move_event)
{
    if (move_event.type != sf::Event::MouseMoved)
        return;
    // TODO:: optimize!
    if (text_elements_.size() > 0)
        text_elements_.clear();
    position_ = input_handler::get()->convert_mouse_pos(math::vector2f(move_event.mouseMove.x, move_event.mouseMove.y));
    for (unsigned i = 0; i < base_tooltip::get_list().size(); ++i)
    {
        if (base_tooltip::get_list()[i]->is_tt_enabled() &&
            base_tooltip::get_list()[i]->get_tt_bounds().contains(position_.get_sf_vector()))
        {
            create_tooltip(base_tooltip::get_list()[i]);
            break;
        }
    }
}

void tooltip_handler::create_tooltip(base_tooltip* tooltip_ptr)
{
    const utils::yth_node* data = tooltip_ptr->get_tooltip_data();
    sf::Font* font = resource_handler::get()->get_font("text");
    float width = 0.0f, height = 0.0f;
    math::vector2f pos(text_spacing_, text_spacing_);
    switch (tooltip_ptr->get_tooltip_type())
    {
    case tooltip_type::generic:
    {
        sf::Text title(data->child("title")->value(), *font, 15);
        title.setStyle(sf::Text::Bold);
        sf::Text text(utils::wrap_string(data->child("description")->value(), 30), *font, 14);
        sf::Text subtext(data->child("subtext")->value(), *font, 11);
        subtext.setStyle(sf::Text::Italic);
        text_elements_.push_back(title);
        text_elements_.push_back(text);
        text_elements_.push_back(subtext);
        break;
    }
    case tooltip_type::item:
    {
        for (unsigned i = 0; i < data->child_count(); ++i)
        {
            sf::Text text(utils::wrap_string(data->child(i)->value(), 30), *font, 14);
            if (data->child(i)->key() == "name")
            {
                text.setString(utils::wrap_string(data->child(i)->value(), 18));
                text.setCharacterSize(15);
                text.setStyle(sf::Text::Bold);
            }
            else if (data->child(i)->key() == "flavor_text")
            {
                text.setCharacterSize(11);
                text.setStyle(sf::Text::Italic);
            }
            else
            {
                sf::Text label(utils::wrap_string(data->child(i)->key(), 30), *font, 14);
                text_elements_.push_back(label);
            }

            text_elements_.push_back(text);
        }
        break;
    }
    default:
        return;
    }
    for (unsigned i = 0; i < text_elements_.size(); ++i)
    {
        width = text_elements_[i].getLocalBounds().width > width ? text_elements_[i].getLocalBounds().width : width;
        if (tooltip_ptr->get_tooltip_type() == tooltip_type::item)
        {
            if (i%2 || i == 0)
            {
                height += text_elements_[i].getLocalBounds().height + text_spacing_;
            }
        }
        else
        {
            height += text_elements_[i].getLocalBounds().height + text_spacing_;
        }
    }
    if (tooltip_ptr->get_tooltip_type() == tooltip_type::item)
        width = 150;
    width += text_spacing_ * 2;
    height += text_spacing_;
    frame_.setSize(sf::Vector2f(width, height));

    math::vector2f screen_bounds = input_handler::get()->convert_mouse_pos(state_manager::get()->get_dimensions());
    position_.x -= width;
    if (position_.y + height > screen_bounds.y)
    {
        position_.y -= height;
    }
    if (position_.x < screen_bounds.x - state_manager::get()->get_dimensions().x)
    {
        position_.x -= width;
    }

    frame_.setPosition(position_.get_sf_vector());
    for (unsigned i = 0; i < text_elements_.size(); ++i)
    {
        if (tooltip_ptr->get_tooltip_type() == tooltip_type::item)
        {
            if (i%2)
            {
                text_elements_[i].setPosition(position_ + pos);
            }
            else if (i > 0)
            {
                text_elements_[i].setPosition(position_ + math::vector2f(150 - text_elements_[i].getLocalBounds().width, 0.0f) + pos);
                pos.y += text_elements_[i].getLocalBounds().height + text_spacing_;
            }
            else
            {
                text_elements_[i].setPosition(position_ + pos);
                pos.y += text_elements_[i].getLocalBounds().height + text_spacing_;
            }
        }
        else
        {
            text_elements_[i].setPosition(position_ + pos);
            pos.y += text_elements_[i].getLocalBounds().height + text_spacing_;
        }
    }
}
void tooltip_handler::set_status_all(bool status)
{
    for (unsigned i = 0; i < base_tooltip::get_list().size(); ++i)
    {
        base_tooltip::get_list()[i]->set_tt_enabled(status);
    }
}

void tooltip_handler::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (text_elements_.size() > 0)
    {
        target.draw(frame_, states);
        for (unsigned i = 0; i < text_elements_.size(); ++i)
            target.draw(text_elements_[i], states);
    }
}
}   // namespace ui
}   // namespace icarus
