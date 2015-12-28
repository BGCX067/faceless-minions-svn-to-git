#ifndef ICARUS_MATH_HPP_
#define ICARUS_MATH_HPP_

#include <cmath>

namespace icarus
{
namespace math
{
    const double EPSILON = 10e-5;

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
    const T& highest (const T& a, const T& b)
    {
        return (a<b)?b:a;
    }
} // namespace math
} // namespace icarus

#endif // ICARUS_MATH_HPP_
