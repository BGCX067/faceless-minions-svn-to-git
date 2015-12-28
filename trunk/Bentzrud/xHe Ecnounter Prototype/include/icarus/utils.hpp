
#ifndef ICARUS_utils_HPP_
#define ICARUS_utils_HPP_

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <SFML/Graphics.hpp>

namespace icarus
{
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
