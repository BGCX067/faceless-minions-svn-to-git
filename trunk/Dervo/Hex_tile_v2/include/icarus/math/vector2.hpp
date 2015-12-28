#ifndef ICARUS_MATH_VECTOR2_HPP_
#define ICARUS_MATH_VECTOR2_HPP_

#include <iostream>

#include <SFML/Graphics.hpp>

#include "icarus/math.hpp"

namespace icarus
{
namespace math
{
template<typename T>
struct vector2
{
    T x;
    T y;

    vector2() : x(static_cast<T>(0.0)),
                y(static_cast<T>(0.0))
    {
    }
    vector2(T X, T Y) : x(X), y(Y)
    {
    }
    vector2(const vector2<T>& vec) : x(vec.x), y(vec.y)
    {
    }
    vector2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y)
    {
    }
    sf::Vector2<T> get_sf_vector() const
    {
        return sf::Vector2<T>(x, y);
    }

    T get_angle_to(const vector2<T>& vec)
    {
        return atan2(vec.x - x, vec.y - y);
    }

    bool is_nonzero() const
    {
        return (x||y);
    }

    T length()
    {
        return sqrt(x*x+y*y);
    }

    vector2<T> normalized()
    {
        return *this/length();
    }

    T cross(const vector2<T>& b)
    {
        return (x*b.y) - (y*b.x);
    }

    vector2<T> operator-() const
    {
        return vector2<T>(-(x), -(y));
    }
    vector2<T> operator+(const vector2<T>& vec) const
    {
        return vector2<T>(x + vec.x, y + vec.y);
    }
    vector2<T> operator+(const T value) const
    {
        return vector2<T>(x + value, y + value);
    }
    vector2<T> operator-(const vector2<T>& vec) const
    {
        return vector2<T>(x - vec.x, y - vec.y);
    }
    vector2<T> operator-(const T value) const
    {
        return vector2<T>(x - value, y - value);
    }
    vector2<T> operator*(const vector2<T>& vec) const
    {
        return vector2<T>(x * vec.x, y * vec.y);
    }
    vector2<T> operator*(const T value) const
    {
        return vector2<T>(x * value, y * value);
    }
    vector2<T> operator/(const vector2<T>& vec) const
    {
        return vector2<T>(x / vec.x, y / vec.y);
    }
    vector2<T> operator/(const T value) const
    {
        return vector2<T>(x / value, y / value);
    }
    vector2<T>& operator+=(const T value)
    {
        return *this = *this + value;
    }
    vector2<T>& operator+=(const vector2<T>& vec)
    {
        return *this = *this + vec;
    }
    vector2<T>& operator-=(const T value)
    {
        return *this = *this - value;
    }
    vector2<T>& operator-=(const vector2<T>& vec)
    {
        return *this = *this - vec;
    }
    vector2<T>& operator*=(const T value)
    {
        return *this = *this * value;
    }
    vector2<T>& operator*=(const vector2<T>& vec)
    {
        return *this = *this * vec;
    }
    vector2<T>& operator/=(const T value)
    {
        return *this = *this / value;
    }
    vector2<T>& operator/=(const vector2<T>& vec)
    {
        return *this = *this / vec;
    }
    bool operator<(const vector2<T>& vec) const
    {
        return ((x + y) <  (vec.x + vec.y));
    }
    bool operator<=(const vector2<T>& vec) const
    {
        return ((x + y) <= (vec.x + vec.y));
    }
    bool operator>(const vector2<T>& vec) const
    {
        return ((x + y) >  (vec.x + vec.y));
    }
    bool operator>=(const vector2<T>& vec) const
    {
        return ((x + y) >= (vec.x + vec.y));
    }
    bool operator==(const vector2<T>& vec) const
    {
        return (compare(x, vec.x) &&  compare(y, vec.y));
    }
    bool operator!=(const vector2<T>& vec) const
    {
        return (!compare(x, vec.x) || !compare(y, vec.y));
    }
    operator sf::Vector2<T>() const
    {
        return sf::Vector2<T>(x, y);
    }
};
template<typename T>
static sf::Vector2<T> get_sf_vector(const vector2<T>& vec)
{
    return sf::Vector2<T>(vec.x, vec.y);
}
template<typename T>
static vector2<T> v2lerp(const vector2<T>& value1,
                       const vector2<T>& value2,
                       const T& step)
{
    return value1 + (value2 - value1) * step;
}

typedef vector2<float> vector2f;
typedef vector2<unsigned> vector2u;
typedef vector2<int> vector2i;
typedef vector2<double> vector2d;

template<typename T>
bool point_in_triangle(const vector2<T>& v1,
                       const vector2<T>& v2,
                       const vector2<T>& v3,
                       const vector2<T>& p)
{
    T s = ((v2.y - v3.y) * (p.x - v3.x) + (v3.x - v2.x) * (p.y - v3.y)) /
        ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));
    T t = ((v3.y - v1.y) * (p.x - v3.x) + (v1.x - v3.x) * (p.y - v3.y)) /
        ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));

    //float p = v1 + (v2 - v1) * s + (v3 - v1) * t;
    return (0 <= s && s <= 1 && 0 <= t && t <= 1 && s + t <= 1);
}

template <class T>
bool line_intersect(const math::vector2<T>& a1,
                    const math::vector2<T>& a2,
                    const math::vector2<T>& b1,
                    const math::vector2<T>& b2)
{
    math::vector2<T> r = a2 - a1;
    math::vector2<T> s = b2 - b1;
    T t = (b1 - a1).cross(s) / r.cross(s);
    T u = (b1 - a1).cross(r) / r.cross(s);

    return (0 <= t && t <= 1 && 0 <= u && u <= 1);
}
} // namespace math
} // namespace icarus

#endif // ICARUS_MATH_VECTOR2_HPP_
