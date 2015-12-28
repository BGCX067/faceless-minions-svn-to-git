#ifndef ICARUS_MATH_VECTOR2_HPP_
#define ICARUS_MATH_VECTOR2_HPP_

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
    sf::Vector2<T> get_sf_vector()
    {
        return sf::Vector2<T>(x, y);
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
} // namespace math
} // namespace icarus

#endif // ICARUS_MATH_VECTOR2_HPP_
