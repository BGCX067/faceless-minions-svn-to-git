#include <fstream>
#include "icarus/utils.hpp"

namespace icarus
{
namespace utils
{
bool file_exists(const std::string& path)
{
    std::ifstream ifile(path.c_str());
    return ifile;
}
bool is_number(const std::string& str)
{
    return (!str.empty() && str.find_first_not_of("0123456789.-") == std::string::npos);
}

uint8_t rgb::r() const
{
    return static_cast<uint8_t>(0xFF & val_ >> 16);
}
uint8_t rgb::g() const
{
    return static_cast<uint8_t>(0xFF & val_ >> 8);
}
uint8_t rgb::b() const
{
    return static_cast<uint8_t>(0xFF & val_);
}
rgb::operator sf::Color() const
{
    return sf::Color(r(), g(), b());
}

uint8_t argb::a() const
{
    return static_cast<uint8_t>(0xFF & val_ >> 24);
}
argb::operator sf::Color() const
{
    return sf::Color(r(), g(), b(), a());
}
}   // namespace utils
}   // namespace icarus
