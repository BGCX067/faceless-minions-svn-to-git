#include <cmath>
#include "icarus/overworld/item.hpp"
#include "icarus/utils/yth_handler.hpp"
#include "icarus/resource_handler.hpp"

namespace icarus
{
namespace overworld
{
item::item()
:
    name_(""),
    display_name_(""),
    flavortext_(""),
    item_class_(item_class::weapon),
    base_index_(0),
    prefix_index_(-1),
    postfix_index_(-1),
    is_unique_(false),
    damage_(0),
    damage_min_(0),
    damage_max_(0),
    damage_mod_(0.0f),
    crit_chance_(0),
    crit_mult_(0.0f),
    dodge_chance_(0),
    block_chance_(0),
    block_mult_(0.0f),
    resist_chance_(0),
    resist_mult_(0.0f),
    health_bonus_(0),
    health_regen_(0),
    strength_bonus_(0),
    intelligence_bonus_(0),
    agility_bonus_(0)
{
}
item::~item()
{
}

void item::generate_random(unsigned level)
{
    is_unique_ = false;
    utils::yth_node* item_root = resource_handler::get()->get_root_node("items");
    item_class_ = static_cast<item_class::value>(floor(rand()%4));
    switch (item_class_)
    {
        case item_class::weapon: item_root = item_root->child("weapons"); break;
        case item_class::armor: item_root = item_root->child("armor"); break;
        case item_class::misc: item_root = item_root->child("misc"); break;
        case item_class::hat: item_root = item_root->child("hats"); break;
        default: break;
    }
    reset_stats();
    unsigned base_index = floor(rand()%item_root->child("base_items")->child_count());
    base_index_ = base_index;
    name_ = item_root->child("base_items")->child(base_index)->key();
    std::string base_name = "";
    if (item_root->child("base_items")->child(base_index)->child_count("name"))
        base_name = item_root->child("base_items")->child(base_index)->child("name")->value();
    apply_stats(item_root->child("base_items")->child(base_index));
    icon_name_ = item_root->child("base_items")->child(base_index)->child("icon")->value();

    std::string pre = "";
    std::string post = "";
    if (rand()%100 > 50 && item_root->child("affixes")->child_count() > 0)
    {
        unsigned pre_index = floor(rand()%item_root->child("affixes")->child_count());
        pre = item_root->child("affixes")->child(pre_index)->child("pretext")->value();
        apply_stats(item_root->child("affixes")->child(pre_index));
        prefix_index_ = pre_index;
    }
    if (rand()%100 > 50 && item_root->child("affixes")->child_count() > 0)
    {
        unsigned post_index = floor(rand()%item_root->child("affixes")->child_count());
        post = item_root->child("affixes")->child(post_index)->child("posttext")->value();
        apply_stats(item_root->child("affixes")->child(post_index));
        postfix_index_ = post_index;
    }
    display_name_ = base_name;
    if (pre != "")
        display_name_ = pre + " " + display_name_;
    if (post != "")
        display_name_ = display_name_ + (post[0] == ',' ? "" : " ") + post;

    float level_mult = float(level)/10.0;
    // offense
    damage_ *= level_mult;
    damage_min_ *= level_mult;
    damage_max_ *= level_mult;
    damage_mod_ *= level_mult;
    crit_chance_ *= level_mult;
    crit_mult_ *= level_mult;

    // defense
    dodge_chance_ *= level_mult;
    block_chance_ *= level_mult;
    block_mult_ *= level_mult;
    resist_chance_ *= level_mult;
    resist_mult_ *= level_mult;
    health_bonus_ *= level_mult;
    health_regen_ *= level_mult;

    // stats
    strength_bonus_ *= level_mult;
    intelligence_bonus_ *= level_mult;
    agility_bonus_ *= level_mult;
    setup_tt_data();
}
void item::get_random_unique()
{
    utils::yth_node* unique_list = resource_handler::get()->get_root_node("uniques");
    int index = floor(rand()%unique_list->child_count());
    get_unique(unique_list->child(index)->key());
}
void item::get_unique(const std::string& name)
{
    is_unique_ = true;
    utils::yth_node* item_node = resource_handler::get()->get_root_node("uniques");
    if (item_node->child_count(name) <= 0)
    {
        get_random_unique();
        return;
    }
    for (unsigned i = 0; i < item_node->child_count(); ++i)
        if (item_node->child(i)->key() == name)
            base_index_ = i;

    item_node = item_node->child(name);

    name_ = item_node->key();
    icon_name_ = item_node->key();
    if (item_node->child_count("name"))
        display_name_ = item_node->child("name")->value();
    if (item_node->child_count("flavortext"))
        flavortext_ = item_node->child("flavortext")->value();

    if (item_node->child_count("class"))
    {
        if (item_node->child("class")->value() == "weapon") item_class_ = item_class::weapon;
        if (item_node->child("class")->value() == "armor") item_class_ = item_class::armor;
        if (item_node->child("class")->value() == "misc") item_class_ = item_class::misc;
        if (item_node->child("class")->value() == "hat") item_class_ = item_class::hat;
    }
    reset_stats();
    apply_stats(item_node);
    setup_tt_data();
}

void item::apply_stats(utils::yth_node* item_node)
{
    std::vector<std::string> splitvec;
    if (item_node->child_count("damage"))
    {
        utils::split_str(item_node->child("damage")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        damage_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("damage_min"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("damage_min")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        damage_min_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("damage_max"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("damage_max")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        damage_max_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("damage_mod"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("damage_mod")->value(), "-", splitvec);
        float v_min = atof(splitvec[0].c_str());
        float v_max = atof(splitvec[1].c_str());
        damage_mod_ += (v_max <= v_min ? v_max : (float)rand()/((float)RAND_MAX/(v_max-v_min)) + v_min);
    }
    if (item_node->child_count("crit_chance"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("crit_chance")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        crit_chance_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("crit_mult"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("crit_mult")->value(), "-", splitvec);
        float v_min = atof(splitvec[0].c_str());
        float v_max = atof(splitvec[1].c_str());
        crit_mult_ += (v_max <= v_min ? v_max : (float)rand()/((float)RAND_MAX/(v_max-v_min)) + v_min);
    }
    if (item_node->child_count("dodge_chance"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("dodge_chance")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        dodge_chance_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("block_chance"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("block_chance")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        block_chance_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("block_mult"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("block_mult")->value(), "-", splitvec);
        float v_min = atof(splitvec[0].c_str());
        float v_max = atof(splitvec[1].c_str());
        block_mult_ += (v_max <= v_min ? v_max : (float)rand()/((float)RAND_MAX/(v_max-v_min)) + v_min);
    }
    if (item_node->child_count("resist_chance"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("resist_chance")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        resist_chance_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("resist_mult"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("resist_mult")->value(), "-", splitvec);
        float v_min = atof(splitvec[0].c_str());
        float v_max = atof(splitvec[1].c_str());
        resist_mult_ += (v_max <= v_min ? v_max : (float)rand()/((float)RAND_MAX/(v_max-v_min)) + v_min);
    }
    if (item_node->child_count("health_bonus"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("health_bonus")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        health_bonus_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("health_regen"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("health_regen")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        health_regen_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("strength"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("strength")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        strength_bonus_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("intelligence"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("intelligence")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        intelligence_bonus_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
    if (item_node->child_count("agility"))
    {
        splitvec.clear();
        utils::split_str(item_node->child("agility")->value(), "-", splitvec);
        unsigned v_min = atoi(splitvec[0].c_str());
        unsigned v_max = atoi(splitvec[1].c_str());
        agility_bonus_ += (v_max <= v_min ? v_max : rand()%(v_max-v_min) + v_min);
    }
}

void item::reset_stats()
{
    // offense
    damage_ = 0;
    damage_min_ = 0;
    damage_max_ = 0;
    damage_mod_ = 0;
    crit_chance_ = 0;
    crit_mult_ = 0;

    // defense
    dodge_chance_ = 0;
    block_chance_ = 0;
    block_mult_ = 0;
    resist_chance_ = 0;
    resist_mult_ = 0;
    health_bonus_ = 0;
    health_regen_ = 0;

    // stats
    strength_bonus_ = 0;
    intelligence_bonus_ = 0;
    agility_bonus_ = 0;
}

void item::setup_tt_data()
{
    tt_data_node_ = new utils::yth_node("root");
    utils::yth_node* name = new utils::yth_node("name", display_name_);
    tt_data_node_->child_add(name);
    if (damage_max_ != 0)
    {
        std::string damage_text = utils::to_str(damage_min_+damage_)+" - "+utils::to_str(damage_max_+damage_);
        utils::yth_node* damage = new utils::yth_node("Damage:", damage_text);
        tt_data_node_->child_add(damage);
    }
    else if (damage_ != 0)
    {
        utils::yth_node* damage = new utils::yth_node("Damage:", "+"+utils::to_str(damage_));
        tt_data_node_->child_add(damage);
    }
    if (crit_chance_ != 0)
    {
        utils::yth_node* critchance = new utils::yth_node("Critical chance:", "+"+utils::to_str(crit_chance_)+"%");
        tt_data_node_->child_add(critchance);
    }
    if (crit_mult_ != 0)
    {
        utils::yth_node* critmult = new utils::yth_node("Critical damage:", "+"+utils::to_str(int(crit_mult_*100))+"%");
        tt_data_node_->child_add(critmult);
    }
    if (dodge_chance_ != 0)
    {
        utils::yth_node* dodge = new utils::yth_node("Dodge chance:", "+"+utils::to_str(dodge_chance_)+"%");
        tt_data_node_->child_add(dodge);
    }
    if (block_chance_ != 0)
    {
        utils::yth_node* block = new utils::yth_node("Block chance:", "+"+utils::to_str(block_chance_)+"%");
        tt_data_node_->child_add(block);
    }
    if (block_mult_ != 0)
    {
        utils::yth_node* blockm = new utils::yth_node("Block %:", "+"+utils::to_str(int(block_mult_*100))+"%");
        tt_data_node_->child_add(blockm);
    }
    if (resist_chance_ != 0)
    {
        utils::yth_node* res = new utils::yth_node("Resist chance:", "+"+utils::to_str(resist_chance_)+"%");
        tt_data_node_->child_add(res);
    }
    if (resist_mult_ != 0)
    {
        utils::yth_node* resm = new utils::yth_node("Resist %:", "+"+utils::to_str(int(resist_mult_*100))+"%");
        tt_data_node_->child_add(resm);
    }
    if (health_bonus_ != 0)
    {
        utils::yth_node* hp = new utils::yth_node("Health:", "+"+utils::to_str(health_bonus_));
        tt_data_node_->child_add(hp);
    }
    if (health_regen_ != 0)
    {
        utils::yth_node* hpr = new utils::yth_node("Health Regen:", "+"+utils::to_str(health_regen_));
        tt_data_node_->child_add(hpr);
    }
    if (strength_bonus_ != 0)
    {
        utils::yth_node* str = new utils::yth_node("Strength:", "+"+utils::to_str(strength_bonus_));
        tt_data_node_->child_add(str);
    }
    if (intelligence_bonus_ != 0)
    {
        utils::yth_node* intelligence = new utils::yth_node("Intelligence:", "+"+utils::to_str(intelligence_bonus_));
        tt_data_node_->child_add(intelligence);
    }
    if (agility_bonus_ != 0)
    {
        utils::yth_node* agi = new utils::yth_node("Agility:", "+"+utils::to_str(agility_bonus_));
        tt_data_node_->child_add(agi);
    }
    if (flavortext_ != "")
    {
        utils::yth_node* flavor = new utils::yth_node("flavor_text", flavortext_);
        tt_data_node_->child_add(flavor);
    }
}
void item::save(std::ofstream& stream)
{
    utils::item saved_item;
    saved_item.active_ = 1;
    saved_item.unique_ = (is_unique_ ? 1 : 0);
    saved_item.type_ = unsigned(item_class_);
    saved_item.base_ = base_index_;
    saved_item.prefix_ = prefix_index_ +1;
    saved_item.postfix_ = postfix_index_ +1;
    saved_item.mindmg_ = damage_min_;
    saved_item.maxdmg_ = damage_max_;
    saved_item.bonusdmg_ = damage_;
    saved_item.dmgmod_ = unsigned(damage_mod_*100);
    saved_item.critchance_ = crit_chance_;
    saved_item.critmod_ = unsigned(crit_mult_*100);
    saved_item.dodgechance_ = dodge_chance_;
    saved_item.blockchance_ = block_chance_;
    saved_item.blockmod_ = unsigned(block_mult_*100);
    saved_item.reschance_ = resist_chance_;
    saved_item.resmod_ = unsigned(resist_mult_*100);
    saved_item.hpbonus_ = health_bonus_;
    saved_item.hpregen_ = health_regen_;
    saved_item.str_ = strength_bonus_;
    saved_item.int_ = intelligence_bonus_;
    saved_item.agi_ = agility_bonus_;
    stream.write(saved_item.bytes, sizeof(saved_item.bytes));
}
void item::load(std::ifstream& stream)
{

}
void item::load_item(const utils::item& saved_item)
{
    is_unique_ = int(saved_item.unique_);
    item_class_ = item_class::value(saved_item.type_);
    base_index_ = unsigned(saved_item.base_);
    prefix_index_ = int(saved_item.prefix_) -1;
    postfix_index_ = int(saved_item.postfix_) -1;

    if (!is_unique_)
    {
        utils::yth_node* item_root = resource_handler::get()->get_root_node("items");
        switch (item_class_)
        {
            case item_class::weapon: item_root = item_root->child("weapons"); break;
            case item_class::armor: item_root = item_root->child("armor"); break;
            case item_class::misc: item_root = item_root->child("misc"); break;
            case item_class::hat: item_root = item_root->child("hats"); break;
            default: break;
        }
        std::string base_name = "";
        name_ = item_root->child("base_items")->child(base_index_)->key();
        if (item_root->child("base_items")->child(base_index_)->child_count("name"))
            base_name = item_root->child("base_items")->child(base_index_)->child("name")->value();
        icon_name_ = item_root->child("base_items")->child(base_index_)->child("icon")->value();
        std::string pre = "";
        std::string post = "";
        if (prefix_index_ != -1)
        {
            pre = item_root->child("affixes")->child(prefix_index_)->child("pretext")->value();
        }
        if (postfix_index_ != -1)
        {
            post = item_root->child("affixes")->child(postfix_index_)->child("posttext")->value();
        }

        display_name_ = base_name;
        if (pre != "")
            display_name_ = pre + " " + display_name_;
        if (post != "")
            display_name_ = display_name_ + (post[0] == ',' ? "" : " ") + post;
    }
    else
    {
        utils::yth_node* item_node = resource_handler::get()->get_root_node("uniques");
        item_node = item_node->child(base_index_);
        name_ = item_node->key();
        icon_name_ = item_node->key();
        if (item_node->child_count("name"))
            display_name_ = item_node->child("name")->value();
        if (item_node->child_count("flavortext"))
            flavortext_ = item_node->child("flavortext")->value();
    }

    damage_min_ = unsigned(saved_item.mindmg_);
    damage_max_ = unsigned(saved_item.maxdmg_);
    damage_ = unsigned(saved_item.bonusdmg_);
    damage_mod_ = float(int(saved_item.dmgmod_))/100.0f;
    crit_chance_ = unsigned(saved_item.critchance_);
    crit_mult_ = float(int(saved_item.critmod_))/100.0f;
    dodge_chance_ = unsigned(saved_item.dodgechance_);
    block_chance_ = unsigned(saved_item.blockchance_);
    block_mult_ = float(int(saved_item.blockmod_))/100.0f;
    resist_chance_ = unsigned(saved_item.reschance_);
    resist_mult_ = float(int(saved_item.resmod_))/100.0f;
    health_bonus_ = unsigned(saved_item.hpbonus_);
    health_regen_ = unsigned(saved_item.hpregen_);
    strength_bonus_ = unsigned(saved_item.str_);
    intelligence_bonus_ = unsigned(saved_item.int_);
    agility_bonus_ = unsigned(saved_item.agi_);

    setup_tt_data();
}
}   // namespace overworld
}   // namespace icarus
