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

    // Loop through the points in anticlockwise order and draw
    // a vector between consecutive points. If the points is
    // left of every vector, it's in the polygon
    bool contains(const sf::Vector2f& point)
    {
        for(int i = 0; i < points.size(); ++i)
        {
            auto p1 = points[i];
            auto p2 = points[(i+1)%points.size()];
            // To check the side of the line AB, take AB x AX
            // and look at the sign. If +ve, by the right hand rule
            // the point X is to the left of AB
            auto v = p2 - p1;
            auto u = point - p1;
            if(v.x*u.y - v.y*u.x < 0) return false;
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
