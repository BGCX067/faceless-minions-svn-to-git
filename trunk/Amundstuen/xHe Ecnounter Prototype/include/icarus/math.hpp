#ifndef ICARUS_MATH_HPP_
#define ICARUS_MATH_HPP_

#include <cmath>

namespace icarus
{
namespace math
{
    const double EPSILON = 10e-5;
    const double PI      = 3.1415926535897932;

    template<typename T>
    T deg_to_rad(const T degrees)
    {
        return (degrees * (math::PI / 180.0));
    }

    template<typename T>
    T rad_to_deg(const T radians)
    {
        return (radians * (180.0 / math::PI));
    }

    template<typename T>
    T clamp(const T& value, const T& min, const T& max)
    {
        return (value > max ? max : (value < min ? min : value));
    }

    template<typename T>
    bool check_range(T& value, const T& min, const T& max)
    {
        return (value > max ? false : (value < min ? false : true));
    }

    template<typename T>
    bool compare(const T& var1, const T& var2, const T& tolerance = EPSILON)
    {
        T diff = fabs(var1 - var2);
        if (diff <= tolerance)
            return true;
        return false;
    }

    template<typename T>
    T lerp(const T& value1, const T& value2, const T& step)
    {
        return value1 + step * (value2 - value1);
    }

    template <class T>
    const T& lowest (const T& a, const T& b)
    {
        return (a>b)?b:a;
    }

    template <class T>
    const T& lowest (const T& a, const T& b, const T& c)
    {
        return (a > b ? (b > c ? c : b) : (a > c ? c : a));
    }

    template <class T>
    const T& highest (const T& a, const T& b)
    {
        return (a<b)?b:a;
    }

    template <class T>
    const T& highest (const T& a, const T& b, const T& c)
    {
        return (a < b ? (b < c ? c : b) : (a < c ? c : a));
    }

    template <class T>
    T to_precision(const T& val, int decimals)
    {
        return (ceil(val*static_cast<T>(pow(10, decimals))))/
                     static_cast<T>(pow(10, decimals));
    }
} // namespace math
} // namespace icarus

#endif // ICARUS_MATH_HPP_
