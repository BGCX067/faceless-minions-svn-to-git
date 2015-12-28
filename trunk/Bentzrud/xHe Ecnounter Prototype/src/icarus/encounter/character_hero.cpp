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
    modified_hp_(0),
    STR_DAMAGE_MOD(0.01f),
    STR_HEALTH_MOD(0.02f),
    AGI_SPEED_MOD(0.0f),
    AGI_COOLDOWN_MOD(0.0f),
    AGI_DODGE_MOD(0.1f),
    AGI_CRITICAL_MOD(0.03f),
    AGI_RESIST_MOD(0.0f),
    INT_DAMAGE_MOD(0.01f),
    INT_CRITICAL_MOD(0.2f),
    INT_RESIST_MOD(0.0f)
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
            effects_.erase(effects_.begin() + i);
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
    strength_ = data.strength_;
    agility_ = data.agility_;
    intelligence_ = data.intelligence_;

    health_ = data.health_;
    health_ += (modified_hp_ = (strength_ * STR_HEALTH_MOD));
    current_health_ = health_ * data.current_health_;
    if (current_health_ <= 0)
    {
        death();
        render_ = false;
    }

    hp_bar_.set_fill_instant(data.current_health_);
    move_speed_ = data.move_speed_;

    utils::yth_node* class_node = resource_handler::get()->get_root_node("classes");
    utils::yth_node* sprite_node = resource_handler::get()->get_root_node("sprite_sheets");
    class_ = data.class_;
    switch (class_)
    {
    case hero_class::warrior:
        setup_skills("warrior", data.skill_levels_);
        class_node = class_node->child("warrior");
        break;
    case hero_class::mage:
        setup_skills("mage", data.skill_levels_);
        class_node = class_node->child("mage");
        break;
    case hero_class::rogue:
        setup_skills("rogue", data.skill_levels_);
        class_node = class_node->child("rogue");
        break;
    }
    std::string sheet_path = sprite_node->child(class_node->child("sprite_sheet")->value())->child("path")->value();
    utils::yth_node* animation_node = sprite_node->child(class_node->child("sprite_sheet")->value())->child(class_node->key());
    load_sprite(animation_node, sheet_path);

    dodge_chance_ = AGI_DODGE_MOD * agility_;

    block_chance_ = 0.0f; // check for shields
    block_amount_ = 0.5f;

    resist_chance_ = INT_RESIST_MOD * intelligence_;
    resist_amount_ = 0.4f;

    critical_chance_ = INT_CRITICAL_MOD * intelligence_;
    critical_mod_ = 1.0f + AGI_CRITICAL_MOD * agility_;

    phys_damage_mod_ = 1.0f + (STR_DAMAGE_MOD * strength_);
    magic_damage_mod_ = 1.0f + (INT_DAMAGE_MOD * intelligence_);

    setup_portrait(index);
    portrait_->get_hp_bar().set_fill_instant(data.current_health_);
}

void character_hero::setup_skills(const std::string& char_class, const unsigned (&levels)[4])
{
    // TODO:: fix
    utils::yth_node* class_node = resource_handler::get()->get_root_node("classes")->child(char_class); // change to char_class
    utils::yth_node* skill_node = resource_handler::get()->get_root_node("skills");

    for (unsigned i = 0; i < class_node->child_count("skill"); ++i)
    {
        composite_skill temp(class_node->child("skill", i)->value(),
                             skill_node->child(class_node->child("skill", i)->value())->child("display_name")->value(),
                             atof(skill_node->child(class_node->child("skill", i)->value())->child("base_cast_time")->value().c_str()),
                             atof(skill_node->child(class_node->child("skill", i)->value())->child("base_cooldown")->value().c_str()),
                             atoi(skill_node->child(class_node->child("skill", i)->value())->child("range")->value().c_str()),
                             atoi(skill_node->child(class_node->child("skill", i)->value())->child("ammunition_cost")->value().c_str()),
                             atoi(skill_node->child(class_node->child("skill", i)->value())->child("flags")->value().c_str()));
        temp.setup_from_data(skill_node->child(class_node->child("skill", i)->value()));
        active_skills_[i] = temp;
    }
}

void character_hero::setup_portrait(unsigned index)
{
    sf::Sprite* temp_portrait = new sf::Sprite;
    temp_portrait->setTexture(*resource_handler::get()->get_texture("images/interface/portrait_dummy.png"));
    portrait_ = new ui::portrait(index, *temp_portrait);
}
void character_hero::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!render_)
        return;
    const sf::Shader* default_shader = states.shader;
    if (flash_)
    {
        resource_handler::get()->get_shader("color_overlay")->setParameter("overlay_color", utils::rgb(0xFFFFFF));
        resource_handler::get()->get_shader("color_overlay")->setParameter("texture", *character_sprite_->getTexture());
        states.shader = resource_handler::get()->get_shader("color_overlay");
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
                         character_sprite_->getLocalBounds().height * 0.4f);
    cast_bar_.set_position(pos);
}
}   // namespace encounter
}   // namespace icarus
