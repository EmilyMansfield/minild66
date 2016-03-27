#ifndef VECMATH_HPP
#define VECMATH_HPP

#include <SFML/System.hpp>
#include <cmath>

namespace vecmath
{
    const float pi = 3.1415926535897;

    template<typename T>
T norm(const sf::Vector2<T>& a)
{
    return std::sqrt(a.x*a.x + a.y*a.y);
}

template<typename T>

T dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
    return a.x*b.x + a.y*b.y;
}
}

#endif /* VECMATH_HPP */
