#ifndef PATHFINDING_HELPER_HPP
#define PATHFINDING_HELPER_HPP

#include <SFML/System.hpp>
#include "navgraph.hpp"
#include "vecmath.hpp"

class PathfindingHelper
{
private:
    Graph<sf::Vector2u>* graph;

public:

    // Target position to aim for
    // Must be contained within the navgraph for anything to happen
    sf::Vector2f target;
    // Current position
    sf::Vector2f pos;

    PathfindingHelper() {}
    PathfindingHelper(const sf::Vector2f& mPos, const sf::Vector2f& mTarget,
            Graph<sf::Vector2u>* mNavgraph) :
        graph(mNavgraph),
        target(mTarget),
        pos(mPos) {}

    void update(float speed)
    {
        // Move in a straight line
        auto v = target - pos;
        float norm = vecmath::norm(v);
        if(norm > 0.1)
        {
            pos += v / norm * speed;
        }
    }
};

#endif /* PATHFINDING_HELPER_HPP */
