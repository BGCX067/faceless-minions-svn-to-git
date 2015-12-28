#include "icarus/encounter/character_hero.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
character_hero::character_hero(grid* grid_ptr,
                               std::string name)
:
    character_base(grid_ptr, name),
    strength_(0),
    intelligence_(0),
    agility_(0),
    str_mod_(0),
    int_mod_(0),
    agi_mod_(0),
    hover_(false)
{
}

character_hero::~character_hero()
{

}

void character_hero::update(double delta)
{
    for (unsigned i = 0; i < 5; ++i)
    {
        active_skills_[i].update_cooldown(delta);
        if (active_skills_[i].is_ready())
            portrait_->set_skill_ready(i, true);
        else
            portrait_->set_skill_ready(i, false);
    }

    for (unsigned i = 0; i < effects_.size(); ++i)
    {
        effects_[i].time_left -= delta;
        if (effects_[i].time_left <= effects_[i].tick_step *
                                     (effects_[i].ticks_remaining - 1))
        {
            effects_[i].ticks_remaining--;
            apply_effect(effects_[i].effect);
        }
        if (effects_[i].time_left <= 0.0f)
        {
            remove_effect(effects_[i].effect);
            effects_.erase(effects_.begin() + i);
        }
    }
    combat_text_.update(delta);
    hp_bar_.update_bar(delta);
    portrait_->get_hp_bar().update_bar(delta);
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height * 0.9f);
    hp_bar_.set_position(pos);
}

void character_hero::setup_from_data(const hero_data& data, unsigned index)
{
    name_ = data.name_;
    surname_ = data.surname_;
    strength_ = data.strength_;
    agility_ = data.agility_;
    intelligence_ = data.intelligence_;

    health_ = data.health_;
    current_health_ = data.current_health_;
    if (current_health_ <= 0)
    {
        death();
        render_ = false;
    }

    hp_bar_.set_fill_instant(current_health_ / health_);
    move_speed_ = data.move_speed_;

    weapon_damage_ = data.base_damage_;

    utils::yth_node* class_node = resource_handler::get()->get_root_node("classes");
    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets");
    class_ = data.class_;
    switch (class_)
    {
    case hero_class::warrior:
        setup_skills("warrior", data.skill_levels_, data.autocast_);
        class_node = class_node->child("warrior");
        break;
    case hero_class::mage:
        setup_skills("mage", data.skill_levels_, data.autocast_);
        class_node = class_node->child("mage");
        break;
    case hero_class::rogue:
        setup_skills("rogue", data.skill_levels_, data.autocast_);
        class_node = class_node->child("rogue");
        break;
    }
    std::string sheet_path = sprite_node->child(class_node->child("sprite_sheet")->value())->child("path")->value();
    utils::yth_node* animation_node = sprite_node->child(class_node->child("sprite_sheet")->value())->child(class_node->key());
    load_sprite(animation_node, sheet_path);

    dodge_chance_ = attribute_mod::AGI_DODGE_MOD * agility_;

    block_chance_ = 0.0f; // check for shields
    block_amount_ = 0.5f;

    resist_chance_ = attribute_mod::INT_RESIST_MOD * intelligence_;
    resist_amount_ = 0.4f;

    critical_chance_ = attribute_mod::INT_CRITICAL_MOD * intelligence_;
    critical_mod_ = 1.0f + attribute_mod::AGI_CRITICAL_MOD * agility_;

    phys_damage_mod_ = 1.0f + (attribute_mod::STR_DAMAGE_MOD * strength_);
    magic_damage_mod_ = 1.0f + (attribute_mod::INT_DAMAGE_MOD * intelligence_);

    setup_portrait(index, class_node);
    portrait_->get_hp_bar().set_fill_instant(float(current_health_) / float(health_));

    utils::yth_node* shadow_node = resource_handler::get()->get_root_node("effects")->child("shadows")->child("medium");
    sheet_path = sprite_node->child(shadow_node->child("sprite_sheet")->value())->child("path")->value();
    shadow_.load_sprite(sprite_node->child(shadow_node->child("sprite_sheet")->value())->child(shadow_node->child("sprite_name")->value()), sheet_path);
}

void character_hero::get_data(hero_data& data)
{
    data.current_health_ = current_health_;
    data.health_ = health_;
    data.kills_ = kills_;
    data.assists_ = assists_;
    data.largest_given_ = largest_given_;
    data.largest_taken_ = largest_taken_;
    data.damage_done_ = damage_dealt_;
    data.damage_taken_ = damage_received_;
    data.friendly_fire_ = friendly_fire_;
    for (unsigned i = 0; i < 5; ++i)
        data.autocast_[i] = active_skills_[i].get_autocast();
}

void character_hero::setup_skills(const std::string& char_class, const unsigned (&levels)[4], const bool (&autocast)[5])
{
    // TODO:: fix
    utils::yth_node* class_node = resource_handler::get()->get_root_node("classes")->child(char_class); // change to char_class
    utils::yth_node* skill_node = resource_handler::get()->get_root_node("skills");

    for (unsigned i = 0; i < class_node->child_count("skill"); ++i)
    {
        composite_skill temp(class_node->child("skill", i)->value(),
                             skill_node->child(class_node->child("skill", i)->value())->child("display_name")->value(),
                             atoi(skill_node->child(class_node->child("skill", i)->value())->child("ammunition_cost")->value().c_str()),
                             atoi(skill_node->child(class_node->child("skill", i)->value())->child("flags")->value().c_str()),
                             (i > 0 ? levels[i-1] : 0));
        temp.setup_from_data(skill_node->child(class_node->child("skill", i)->value()));
        temp.apply_cooldown();
        active_skills_[i] = temp;
        active_skills_[i].set_autocast(autocast[i]);
    }
}

void character_hero::setup_portrait(unsigned index, utils::yth_node* class_node)
{
    sf::Sprite* temp_portrait = new sf::Sprite;
    temp_portrait->setTexture(*resource_handler::get()->get_texture(class_node->child("portrait")->value()));
    portrait_ = new ui::portrait(index, name_, *temp_portrait);
}
void character_hero::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!render_)
        return;
    target.draw(shadow_, states);
    const sf::Shader* default_shader = states.shader;
    if (flash_)
    {
        resource_handler::get()->get_shader("color_overlay")->setParameter("overlay_color", utils::rgb(0xFF0000));
        resource_handler::get()->get_shader("color_overlay")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("color_overlay");
    }
    else if (hover_)
    {
        resource_handler::get()->get_shader("outline")->setParameter("outline_color", utils::rgb(0xFFFF00));
        resource_handler::get()->get_shader("outline")->setParameter("outline_width", 1.0);
        resource_handler::get()->get_shader("outline")->setParameter("alpha", 0.7);
        resource_handler::get()->get_shader("outline")->setParameter("img_width", character_sprite_->getTexture()->getSize().x);
        resource_handler::get()->get_shader("outline")->setParameter("img_height", character_sprite_->getTexture()->getSize().y);
        resource_handler::get()->get_shader("outline")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("outline");
    }
    target.draw(*character_sprite_, states);
    states.shader = default_shader;
    if (is_casting_ && show_cast_bar_)
        target.draw(cast_bar_, states);
    if (show_health_bar_)
        target.draw(hp_bar_, states);
    target.draw(combat_text_, states);
    target.draw(*portrait_, states);
}
void character_hero::update_hp_bar(float fraction)
{
    hp_bar_.set_bar_fill(fraction);
    portrait_->get_hp_bar().set_bar_fill(fraction);
}
void character_hero::update_cast_bar(float remaining, float duration)
{
    cast_bar_.set_label(utils::to_str(math::to_precision(remaining, 1)) + "s");
    cast_bar_.set_bar_fill(math::clamp(1.0f - (remaining / duration), 0.0f, 1.0f));
    portrait_->get_cast_bar().set_bar_fill(math::clamp(1.0f - (remaining / duration), 0.0f, 1.0f));
    math::vector2f pos = get_world_pos() - math::vector2f(0,
                         character_sprite_->getLocalBounds().height);
    cast_bar_.set_position(pos);
}
const sf::Texture* character_hero::get_portrait_ptr()
{
    return portrait_->get_portrait_ptr();
}

std::string character_hero::get_name(unsigned char_limit) const
{
    if (char_limit == 0 || std::string(name_+" "+surname_).size() <= char_limit)
        return std::string(name_+" "+surname_);
    else
        return name_;
}
}   // namespace encounter
}   // namespace icarus
