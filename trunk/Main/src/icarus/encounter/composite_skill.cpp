#include "icarus/encounter/composite_skill.hpp"
#include "icarus/utils.hpp"
#include "icarus/sound.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace encounter
{
composite_skill::composite_skill(const std::string& name,
                                 const std::string& display_name,
                                 int ammunition,
                                 uint16_t flags,
                                 unsigned level)
:
    name_(name),
    display_name_(display_name),
    current_cooldown_(0.0f),
    ammunition_cost_(ammunition),
    autocast_(false),
    flags_(flags),
    level_(level),
    requires_los_(true),
    is_teleport_(false)
{
    is_leveled_ = bool(flags & skill_flags::IS_LEVELED);
    is_ultimate_ = bool(flags & skill_flags::IS_ULTIMATE);
}

composite_skill::~composite_skill()
{
}

void composite_skill::add_subskill(targeted_skill subskill)
{
    subskills_.push_back(subskill);
}
void composite_skill::add_effect(applied_effect effect)
{
    if (subskills_.size() != 0)
        subskills_.back().effect = effect;
}

void composite_skill::update_cooldown(double delta)
{
    if (current_cooldown_ > 0.0f)
        current_cooldown_ -= delta;
}

void composite_skill::setup_from_data(utils::yth_node* skill_node)
{
    std::vector<std::string> split;
    if (is_leveled_ && level_ > 0)
    {
        utils::split_str(skill_node->child("base_cast_time")->value(), "/", split);
        cast_time_ = atof(split[level_-1].c_str());
        split.clear();
        utils::split_str(skill_node->child("base_cooldown")->value(), "/", split);
        cooldown_ = atof(split[level_-1].c_str());
        split.clear();
        utils::split_str(skill_node->child("range")->value(), "/", split);
        range_ = atof(split[level_-1].c_str());
    }
    else if (!is_leveled_)
    {
        cast_time_ = atof(skill_node->child("base_cast_time")->value().c_str());
        cooldown_ = atof(skill_node->child("base_cooldown")->value().c_str());
        range_ = atoi(skill_node->child("range")->value().c_str());
    }
    needs_target_ = static_cast<bool>(range_ > 0);
    if (skill_node->child_count("animation") > 0)
    {
        animation_ = skill_node->child("animation")->value();
    }
    else
    {
        animation_ = "";
    }
    if (skill_node->child_count("requires_los") > 0)
        requires_los_ = (skill_node->child("requires_los")->value() == "true");

    utils::yth_node* sfx_node = resource_handler::get()->get_root_node("audio")->child("sfx");
    if (skill_node->child_count("sfx") == 1)
        sound::get()->load_sfx(sfx_node->child(skill_node->child("sfx")->value())->value(), skill_node->child("sfx")->value());
    else
        for (unsigned i = 0; i < skill_node->child_count("sfx"); ++i)
            sound::get()->load_sfx(sfx_node->child(skill_node->child("sfx", i)->value())->value(), skill_node->child("sfx", i)->value() + utils::to_str(i));
    for (unsigned k = 0; k < skill_node->child_count("subskill"); ++k)
    {
        targeted_skill subskill;
        subskill.name = skill_node->child("subskill", k)->child("name")->value();
        subskill.flags = static_cast<uint16_t>(atoi(skill_node->child("subskill", k)->child("flags")->value().c_str()));

        if (is_leveled_ && level_ > 0)
        {
            split.clear();
            utils::split_str(skill_node->child("subskill", k)->child("base_damage")->value(), "/", split);
            subskill.damage = atoi(split[level_-1].c_str());
            split.clear();
            utils::split_str(skill_node->child("subskill", k)->child("crit_mod")->value(), "/", split);
            subskill.crit_chance_mod = atof(split[level_-1].c_str());
            split.clear();
            utils::split_str(skill_node->child("subskill", k)->child("radius")->value(), "/", split);
            subskill.radius = atoi(split[level_-1].c_str());
        }
        else if (!is_leveled_)
        {
            subskill.damage = atoi(skill_node->child("subskill", k)->child("base_damage")->value().c_str());
            subskill.crit_chance_mod = atof(skill_node->child("subskill", k)->child("crit_mod")->value().c_str());
            subskill.radius = atoi(skill_node->child("subskill", k)->child("radius")->value().c_str());
        }

        if (skill_node->child("subskill", k)->child_count("projectile"))
            subskill.projectile = skill_node->child("subskill", k)->child("projectile")->value();
        if (skill_node->child("subskill", k)->child_count("visual_effect"))
            subskill.visual_effect = skill_node->child("subskill", k)->child("visual_effect")->value();

        if (skill_node->child("subskill", k)->child("type")->value() == "instant") subskill.type = skill_type::instant;
        else if (skill_node->child("subskill", k)->child("type")->value() == "projectile") subskill.type = skill_type::projectile;
        else if (skill_node->child("subskill", k)->child("type")->value() == "railgun") subskill.type = skill_type::railgun;
        else if (skill_node->child("subskill", k)->child("type")->value() == "self") subskill.type = skill_type::self;
        else if (skill_node->child("subskill", k)->child("type")->value() == "teleport") subskill.type = skill_type::teleport;
        else if (skill_node->child("subskill", k)->child("type")->value() == "self_aoe") subskill.type = skill_type::self_aoe;
        else subskill.type = skill_type::undefined;

        is_teleport_ = (subskill.type == skill_type::teleport ? true : is_teleport_);


        if (skill_node->child("subskill", k)->child_count("channeled_duration") > 0)
            subskill.channeled_duration = atof(skill_node->child("subskill", k)->child("channeled_duration")->value().c_str());
        if (skill_node->child("subskill", k)->child_count("channeled_delay") > 0)
            subskill.channeled_delay = atof(skill_node->child("subskill", k)->child("channeled_delay")->value().c_str());
        if (skill_node->child("subskill", k)->child_count("channeled_delay_mod") > 0)
            subskill.channeled_delay_mod = atof(skill_node->child("subskill", k)->child("channeled_delay_mod")->value().c_str());
        if (skill_node->child("subskill", k)->child_count("animation") > 0)
            subskill.animation = skill_node->child("subskill", k)->child("animation")->value();

        if (skill_node->child("subskill", k)->child_count("no_range") > 0 &&
            skill_node->child("subskill", k)->child("no_range")->value() == "true")
        {
            subskill.range = 0;
        }
        else
        {
            subskill.range = range_;
        }
        if (skill_node->child("subskill", k)->child_count("delay") > 0)
        {
            subskill.delay = atof(skill_node->child("subskill", k)->child("delay")->value().c_str());
        }

        if (skill_node->child("subskill", k)->child_count("projectile_speed") > 0)
            subskill.projectile_speed = atof(skill_node->child("subskill", k)->child("projectile_speed")->value().c_str());
        if (skill_node->child("subskill", k)->child_count("projectile_interval") > 0)
            subskill.projectile_interval = atof(skill_node->child("subskill", k)->child("projectile_interval")->value().c_str());
        if (skill_node->child("subskill", k)->child_count("hide_character") > 0)
            subskill.hide_character = (skill_node->child("subskill", k)->child("hide_character")->value() == "true");

        if (skill_node->child("subskill", k)->child_count("effect"))
        {
            applied_effect effect;
            effect.name = skill_node->child("subskill", k)->child("effect")->child("name")->value();
            effect.flags = static_cast<uint16_t>(atoi(skill_node->child("subskill", k)->child("effect")->child("flags")->value().c_str()));

            if (is_leveled_ && level_ > 0)
            {
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("base_damage")->value(), "/", split);
                effect.damage = atoi(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("base_duration")->value(), "/", split);
                effect.duration = atof(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("ticks")->value(), "/", split);
                effect.ticks = atoi(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("int_mod")->value(), "/", split);
                effect.int_mod = atoi(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("agi_mod")->value(), "/", split);
                effect.agi_mod = atoi(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("str_mod")->value(), "/", split);
                effect.str_mod = atoi(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("speed_mod")->value(), "/", split);
                effect.speed_mod = atof(split[level_-1].c_str());
                split.clear();
                utils::split_str(skill_node->child("subskill", k)->child("effect")->child("damage_mod")->value(), "/", split);
                effect.damage_mod = atof(split[level_-1].c_str());
                split.clear();
                if (skill_node->child("subskill", k)->child("effect")->child_count("damage_reduction"))
                {
                    utils::split_str(skill_node->child("subskill", k)->child("effect")->child("damage_reduction")->value(), "/", split);
                    effect.damage_reduction = atof(split[level_-1].c_str());
                }
                split.clear();
                if (skill_node->child("subskill", k)->child("effect")->child_count("dodge_mod"))
                {
                    utils::split_str(skill_node->child("subskill", k)->child("effect")->child("dodge_mod")->value(), "/", split);
                    effect.dodge_mod = atof(split[level_-1].c_str());
                }
            }
            else if (!is_leveled_)
            {
                effect.damage = atoi(skill_node->child("subskill", k)->child("effect")->child("base_damage")->value().c_str());
                effect.duration = atof(skill_node->child("subskill", k)->child("effect")->child("base_duration")->value().c_str());
                effect.ticks = atoi(skill_node->child("subskill", k)->child("effect")->child("ticks")->value().c_str());
                effect.int_mod = atoi(skill_node->child("subskill", k)->child("effect")->child("int_mod")->value().c_str());
                effect.agi_mod = atoi(skill_node->child("subskill", k)->child("effect")->child("agi_mod")->value().c_str());
                effect.str_mod = atoi(skill_node->child("subskill", k)->child("effect")->child("str_mod")->value().c_str());
                effect.speed_mod = atof(skill_node->child("subskill", k)->child("effect")->child("speed_mod")->value().c_str());
                effect.damage_mod = atof(skill_node->child("subskill", k)->child("effect")->child("damage_mod")->value().c_str());
                if (skill_node->child("subskill", k)->child("effect")->child_count("damage_reduction"))
                {
                    effect.damage_reduction = atof(skill_node->child("subskill", k)->child("effect")->child("damage_reduction")->value().c_str());
                }
                if (skill_node->child("subskill", k)->child("effect")->child_count("dodge_mod"))
                {
                    effect.dodge_mod = atof(skill_node->child("subskill", k)->child("effect")->child("dodge_mod")->value().c_str());
                }
            }

            if (skill_node->child("subskill", k)->child("effect")->child_count("stun"))
                effect.stun_ = bool(skill_node->child("subskill", k)->child("effect")->child("stun")->value() == "true");

            if (skill_node->child("subskill", k)->child("effect")->child("type")->value() == "none") effect.type = effect_type::none;
            else if (skill_node->child("subskill", k)->child("effect")->child("type")->value() == "bleeding") effect.type = effect_type::bleeding;
            else if (skill_node->child("subskill", k)->child("effect")->child("type")->value() == "burning") effect.type = effect_type::burning;
            else if (skill_node->child("subskill", k)->child("effect")->child("type")->value() == "frozen") effect.type = effect_type::frozen;
            else if (skill_node->child("subskill", k)->child("effect")->child("type")->value() == "chilled") effect.type = effect_type::chilled;
            else if (skill_node->child("subskill", k)->child("effect")->child("type")->value() == "riposte") effect.type = effect_type::riposte;
            else effect.type = effect_type::undefined;

            subskill.effect = effect;
        }

        // sounds
        if (skill_node->child("subskill", k)->child_count("projectile") > 0)
        {
            utils::yth_node* projectile_node = resource_handler::get()->get_root_node("effects")->child("projectiles")->
                                child(skill_node->child("subskill", k)->child("projectile")->value());

            resource_handler::get()->load_texture(resource_handler::get()->get_root_node("sprite_sheets")->child(projectile_node->child("sprite_sheet")->value())->child("path")->value());
            for (unsigned i = 0; i < projectile_node->child_count("sfx"); ++i)
                sound::get()->load_sfx(sfx_node->child(projectile_node->child("sfx", i)->value())->value(), projectile_node->child("sfx", i)->value());
            if (projectile_node->child_count("impact") > 0)
            {
                sound::get()->load_sfx(sfx_node->child(projectile_node->child("impact")->child("sfx")->value())->value(), projectile_node->child("impact")->child("sfx")->value());
                resource_handler::get()->load_texture(resource_handler::get()->get_root_node("sprite_sheets")->child(projectile_node->child("impact")->child("sprite_sheet")->value())->child("path")->value());
            }
        }
        if (skill_node->child("subskill", k)->child_count("sfx"))
        {
            subskill.sfx = skill_node->child("subskill", k)->child("sfx")->value();
            if (sfx_node->child_count(subskill.sfx) == 1)
                sound::get()->load_sfx(sfx_node->child(subskill.sfx)->value(), subskill.sfx);
            else
                for (unsigned i = 0; i < sfx_node->child_count(subskill.sfx); ++i)
                    sound::get()->load_sfx(sfx_node->child(subskill.sfx, i)->value(), subskill.sfx + utils::to_str(i));
        }
        if (skill_node->child("subskill", k)->child_count("visual_effect") > 0)
        {
            utils::yth_node* effect_node = resource_handler::get()->get_root_node("effects")->child("skill_effects")->
                                child(skill_node->child("subskill", k)->child("visual_effect")->value());

            resource_handler::get()->load_texture(resource_handler::get()->get_root_node("sprite_sheets")->child(effect_node->child("sprite_sheet")->value())->child("path")->value());

        }
        add_subskill(subskill);
    }
}

//void composite_skill::get_subskills(std::vector<targeted_skill>& subskills) const
//{
//    for (unsigned i = 0; i < subskills_.size(); ++i)
//    {
//        subskills.push_back(subskills_[i]);
//    }
//}
}   // namespace encounter
}   // namespace icarus
