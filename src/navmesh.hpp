#ifndef NAVMESH_HPP
#define NAVMESH_HPP

#include <SFML/System.hpp>
#include <vector>
#include <cmath>

#include "graph.hpp"
#include "vecmath.hpp"

class ConvexPolygon
{
public:
    // Points defined in an anticlockwise direction
    // First point must be ac of the positive horizontal
    std::vector<sf::Vector2f> points;

    // Points are defined anticlockwise, so the angle of a ray
    // from a point to each point in that order will increase
    // iff the point lies in the polygon
    bool contains(const sf::Vector2f& point)
    {
        float prev_theta = 0.0f;
        for(auto p : points)
        {
            auto v = p - point;
            float norm = vecmath::norm(v);
            float theta = std::asin(v.y / norm);
            if(theta < 0) theta += 2*vecmath::pi;
            if(theta < prevTheta) return false;
            prev_theta = theta;
        }
        return true;
    }
};

// A Navmesh is a graph of convex polygons
class Navmesh
{
public:
    Graph<ConvexPolygon> polys;

    Navmesh() {}
};

#endif /* NAVMESH_HPP */
