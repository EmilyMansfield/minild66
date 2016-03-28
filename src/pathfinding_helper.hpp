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

    std::list<sf::Vector2u> path;

    // Find the node in the graph that is closest to the given
    // world position
    sf::Vector2u closestNode(const sf::Vector2f& p)
    {
        if(p.x < 0 || p.y < 0)
        {
            throw std::range_error("Target coordinates must be positive");
        }
        sf::Vector2u closest(10000, 10000);
        for(auto e : graph->edges)
        {
            if(vecmath::norm(vecmath::to<float, unsigned int>(e.first)-p)
                < vecmath::norm(vecmath::to<float, unsigned int>(closest)-p))
            {
                closest = e.first;
            }
        }
        return closest;
    }

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
        auto newTargetNode = closestNode(pTarget);
        // Check if clicked node is safe
        if(vecmath::norm(vecmath::to<float,unsigned int>(newTargetNode)-pTarget) > 0.72)
            return;
        // Set the target positions
        targetNode = newTargetNode;
        target = pTarget;
        // Find the nodes closest to the start and end points of
        // the path
        targetNode = closestNode(target);
        posNode = closestNode(pos);
        // Find a path between the start and end points
        path = breadthFirstSearch(graph, posNode, targetNode);
    }

    void update(float speed)
    {
        // If the path is empty or has just one entry it, we
        // should be close enough to just move straight to the
        // actual (and not node) destination.
        if(path.size() < 2)
        {
            auto v = target - pos;
            float norm = vecmath::norm(v);
            if(norm > 0.01) pos += v / norm * speed;
        }
        else
        {
            // Move in a straight line from the current position to the
            // first node in the path
            auto v = vecmath::to<float, unsigned int>(path.front()) - pos;
            float norm = vecmath::norm(v);
            if(norm > 0.1) pos += v / norm * speed;
            // If suitably close to the path node, remove it
            if(vecmath::norm(pos-vecmath::to<float, unsigned int>(path.front())) < 0.1)
            {
                path.pop_front();
            }
        }
    }
};

#endif /* PATHFINDING_HELPER_HPP */
