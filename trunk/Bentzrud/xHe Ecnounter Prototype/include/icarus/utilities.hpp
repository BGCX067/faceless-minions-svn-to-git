
#ifndef ICARUS_UTILITIES_HPP_
#define ICARUS_UTILITIES_HPP_

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace icarus
{
namespace utilities
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
} // namespace utilities
} // namespace icarus

#endif // ICARUS_UTILITIES_HPP_
