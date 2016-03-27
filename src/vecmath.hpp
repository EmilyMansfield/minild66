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

template<typename T>
bool intersect(const sf::Vector2<T>& p0, const sf::Vector2<T>& p1,
        const sf::Vector2<T>& q0, const sf::Vector2<T>& q1,
        sf::Vector2<T>* intersection)
{
    auto dp = sf::Vector2<T>(-(q1.y-q0.y), q1.x-q0.x);
    auto dq = q1 - q0;
    // dp is orthogonal to p1-p0, so if dp.dq = 0 then
    // dp is parallel to dq and there's no intersection
    if(std::abs(vecmath::dot(dp, dq)) < 10e-5) return false;
    float mu = vecmath::dot((p0 - q0), dp) / vecmath::dot(dp, dq);
    // mu is parameter for Q
    intersection->x = q0.x + mu * (q1.x-q0.x);
    intersection->y = q0.y + mu * (q1.y-q0.y);

    return true;
}
}

#endif /* VECMATH_HPP */
