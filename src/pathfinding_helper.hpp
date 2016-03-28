#ifndef PATHFINDING_HELPER_HPP
#define PATHFINDING_HELPER_HPP

#include <SFML/System.hpp>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <iostream>
#include "navgraph.hpp"
#include "vecmath.hpp"

class PathfindingHelper
{
private:
    Graph<sf::Vector2u>* graph;

    sf::Vector2u targetNode;
    sf::Vector2u posNode;

public:

    // Target position to aim for
    // Must be contained within the navgraph for anything to happen
    sf::Vector2f target;
    // Current position
    sf::Vector2f pos;

    PathfindingHelper() {}
    PathfindingHelper(const sf::Vector2f& pPos, const sf::Vector2f& pTarget,
            Graph<sf::Vector2u>* pNavgraph) :
        graph(pNavgraph),
        target(pTarget),
        pos(pPos) {}

    void setTarget(const sf::Vector2f& pTarget)
    {
        target = pTarget;
        if(pTarget.x < 0 || pTarget.y < 0)
        {
            throw std::range_error("Target coordinates must be positive");
        }
        targetNode = sf::Vector2u(10000, 10000);
        for(auto e : graph->edges)
        {
            if(vecmath::norm(vecmath::to<float, unsigned int>(e.first)-pTarget)
                < vecmath::norm(vecmath::to<float, unsigned int>(targetNode)-pTarget))
            {
                targetNode = e.first;
            }
        }
        // Find the node that is closest to the target in O(n)
        // auto it = std::min(graph->edges.begin(), graph->edges.end(),
        //     [&t](
        //         const std::pair<sf::Vector2u, std::vector<sf::Vector2u>>& a,
        //         const std::pair<sf::Vector2u, std::vector<sf::Vector2u>>& b)
        //     {
        //         return vecmath::norm(a.first-t) < vecmath::norm(b.first-t);
        //     });
        // targetNode = it->first;
    }

    void update(float speed)
    {
        // Move in a straight line
        auto v = vecmath::to<float, unsigned int>(targetNode) - pos;
        float norm = vecmath::norm(v);
        if(norm > 0.1)
        {
            pos += v / norm * speed;
        }
    }
};

#endif /* PATHFINDING_HELPER_HPP */
