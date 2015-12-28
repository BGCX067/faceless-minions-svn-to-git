
#ifndef ICARUS_utils_HPP_
#define ICARUS_utils_HPP_

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "icarus/math.hpp"

namespace icarus
{
namespace attribs
{
enum type
{
    intelligence,
    strength,
    agility
};
}   // namespace attribs
namespace utils
{
template<typename T>
std::string to_str(const T& val, int leading_zeroes = 0)
{
    std::stringstream ss;
    if (leading_zeroes != 0)
        ss << std::setfill('0') << std::setw(leading_zeroes) << val;
    else
        ss << val;
    return std::string(ss.str());
}

bool file_exists(const std::string& path);
bool is_number(const std::string& str);
std::string wrap_string(const std::string& str, unsigned chars);
void split_str(const std::string& str,
               const std::string& split,
               std::vector<std::string>& out);

class countdown
{
private:
    float max_time_;
    float time_remaining_;
public:
    countdown(float time) : max_time_(time), time_remaining_(time) {}
    virtual ~countdown() {}
    bool update(double delta) { return ((time_remaining_ -= delta) <= 0.0f); }
    void reset() { time_remaining_ = max_time_; }
    void stop() { time_remaining_ = 0.0f; }
    void set(float time) { max_time_ = time_remaining_ = time; }
    void set(float time, float current) { max_time_ = time; time_remaining_ = current; }
    float get_factor() const { return math::clamp(time_remaining_ / max_time_, 0.0f, 1.0f); }
};

class rgb
{
protected:
    uint32_t val_;
public:
    rgb(uint32_t val) : val_(val) {}
    uint8_t r() const;
    uint8_t g() const;
    uint8_t b() const;
    virtual operator sf::Color() const;
};
class argb : public rgb
{
public:
    argb(uint32_t val) : rgb(val) {}
    uint8_t a() const;
    virtual operator sf::Color() const;
};
} // namespace utils
} // namespace icarus

#endif // ICARUS_utils_HPP_
