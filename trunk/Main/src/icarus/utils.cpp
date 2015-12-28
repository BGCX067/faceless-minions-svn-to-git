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
std::string wrap_string(const std::string& str, unsigned chars)
{
    if (str.length() > chars)
    {
        unsigned split_pos = str.rfind(' ', chars);
        return str.substr(0, split_pos) + "\n" + wrap_string(str.substr(split_pos+1, std::string::npos), chars);
    }
    else
    {
        return str;
    }
}

void split_str(const std::string& str,
               const std::string& split,
               std::vector<std::string>& out)
{
    int pos = 0;
    int found = 0;
    while (str.find(split, pos) != std::string::npos)
    {
        found = str.find(split, pos);
        std::string result = str.substr(pos, found-pos);
        pos = found+1;
        out.push_back(result);
    }
    if (found != 0)
    {
        out.push_back(str.substr(pos));
    }
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

int is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } eint = {0x01020304};

    return eint.c[0] == 1;
}
}   // namespace utils
}   // namespace icarus
