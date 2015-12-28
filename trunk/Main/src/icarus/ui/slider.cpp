#include "icarus/ui/slider.hpp"
#include "icarus/resource_handler.hpp"
#include "icarus/utils.hpp"
#include "icarus/input_handler.hpp"
#include "icarus/math.hpp"

namespace icarus
{
namespace ui
{
slider::slider()
:
    label_enabled_(true),
    min_value_(0),
    max_value_(100),
    drag_enabled_(false),
    drag_min_offset_(0),
    drag_max_offset_(0),
    current_value_(0),
    is_enabled_(true)
{
}
slider::~slider()
{
}

void slider::update_slider(const math::vector2f& pos)
{
    if (pos.x < bar_graphic_.getPosition().x + drag_min_offset_)
    {
        slider_graphic_.setPosition(math::vector2f(bar_graphic_.getPosition().x + drag_min_offset_, slider_graphic_.getPosition().y));
        current_value_ = min_value_;
    }
    else if (pos.x > bar_graphic_.getPosition().x + drag_max_offset_)
    {
        slider_graphic_.setPosition(math::vector2f(bar_graphic_.getPosition().x + drag_max_offset_, slider_graphic_.getPosition().y));
        current_value_ = max_value_;
    }
    else
    {
        unsigned current_pos = pos.x - (bar_graphic_.getPosition().x + drag_min_offset_);
        unsigned max_limit = drag_max_offset_ - drag_min_offset_;
        float step = float(current_pos)/float(max_limit);
        current_value_ = unsigned(math::lerp(float(min_value_), float(max_value_), step));
        slider_graphic_.setPosition(math::vector2f(pos.x, slider_graphic_.getPosition().y));
    }
    value_label_.setString(utils::to_str(current_value_));
}

void slider::setup(utils::yth_node* node, unsigned initial_value)
{
    std::string tex_path = node->child("bar_graphic")->value();
    sf::Texture* tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        bar_graphic_ = sf::Sprite(*tex_ptr);
    tex_path = node->child("slider_graphic")->value();
    tex_ptr  = NULL;
    if ((tex_ptr = resource_handler::get()->get_texture(tex_path)))
        slider_graphic_ = sf::Sprite(*tex_ptr);
    slider_graphic_.setOrigin(slider_graphic_.getLocalBounds().width/2,
                              slider_graphic_.getLocalBounds().height/2);

    drag_min_offset_ = atof(node->child("min_offset")->value().c_str());
    drag_max_offset_ = atof(node->child("max_offset")->value().c_str());

    value_label_.setCharacterSize(20);
    value_label_.setColor(utils::rgb(0xffdd4e));
    value_label_.setFont(*resource_handler::get()->get_font("numbers"));

    set_value(initial_value);
}
void slider::handle_input(const std::vector<sf::Event>& input_events)
{
    for (unsigned i = 0; i < input_events.size(); ++i)
    {
        switch (input_events[i].type)
        {
        case sf::Event::MouseButtonPressed:
            {
                math::vector2f mouse_pos(input_events[i].mouseButton.x, input_events[i].mouseButton.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);

                if (bar_graphic_.getGlobalBounds().contains(mouse_pos) && is_enabled_)
                {
                    drag_enabled_ = true;
                    update_slider(mouse_pos);
                }
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (drag_enabled_)
                drag_enabled_ = false;
            break;
        case sf::Event::MouseMoved:
            {
                math::vector2f mouse_pos(input_events[i].mouseMove.x, input_events[i].mouseMove.y);
                mouse_pos = input_handler::get()->convert_mouse_pos(mouse_pos);

                if (drag_enabled_)
                    update_slider(mouse_pos);
            }
            break;
        default: break;
        }
    }
}

void slider::set_value(unsigned value)
{
    if (value < min_value_)
    {
        slider_graphic_.setPosition(math::vector2f(bar_graphic_.getPosition().x + drag_min_offset_, slider_graphic_.getPosition().y));
        current_value_ = min_value_;
    }
    else if (value > max_value_)
    {
        slider_graphic_.setPosition(math::vector2f(bar_graphic_.getPosition().x + drag_max_offset_, slider_graphic_.getPosition().y));
        current_value_ = max_value_;
    }
    else
    {
        current_value_ = value;
        float step = float(value)/float(max_value_);
        float min_pos = bar_graphic_.getPosition().x + drag_min_offset_;
        float max_pos = bar_graphic_.getPosition().x + drag_max_offset_;
        float x_pos = math::lerp(min_pos, max_pos, step);
        slider_graphic_.setPosition(math::vector2f(x_pos, slider_graphic_.getPosition().y));
    }
    value_label_.setString(utils::to_str(current_value_));
}

void slider::set_position(const math::vector2f& pos)
{
    bar_graphic_.setPosition(pos);
    value_label_.setPosition(pos + math::vector2f(bar_graphic_.getLocalBounds().width + 5.0f, 2.0f));
    slider_graphic_.setPosition(pos.x, pos.y + bar_graphic_.getLocalBounds().height/2);
    set_value(current_value_);
}

void slider::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    const sf::Shader* def = states.shader;
    if (!is_enabled_)
    {
        sf::Shader* bwshader = resource_handler::get()->get_shader("grayscale_luminosity");
        bwshader->setParameter("factor", 0.0f);
        bwshader->setParameter("texture", *bar_graphic_.getTexture());
        states.shader = bwshader;
    }
    target.draw(bar_graphic_, states);
    if (!is_enabled_)
    {
        sf::Shader* bwshader = resource_handler::get()->get_shader("grayscale_luminosity");
        bwshader->setParameter("factor", 0.0f);
        bwshader->setParameter("texture", *slider_graphic_.getTexture());
        states.shader = bwshader;
    }
    target.draw(slider_graphic_, states);
    states.shader = def;
    if (label_enabled_)
        target.draw(value_label_, states);
}
}   // namespace ui
}   // namespace icarus
