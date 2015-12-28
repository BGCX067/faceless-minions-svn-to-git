#include <string>
#include <vector>

namespace icarus
{
namespace overworld
{
enum type_option {ATTACK_, FLEE_, AVOID_, NEGOTIATE_};
enum type_requirement {ROGUE_, BARD_, FIGHTER_, MAGE_, PRIEST_, MERCHANT_};

struct struct_option
{
    type_option option_;
    std::string display_name_;
    type_requirement requirement_;
};
struct struct_scenario
{
    std::string background_;
    std::vector<int> enemy_group; //waves and enemies
};

struct struct_encounter
{
    std::string display_title;
    std::string display_text;
    int xp_reward;
    int possilility;
    std::vector<struct_option>;
    std::vector<struct_scenario>;
};
} // namespace overworld
} // namespace icarus
