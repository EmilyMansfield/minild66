#ifndef NAVGRAPH_HPP
#define NAVGRAPH_HPP

#include <set>
#include <vector>
#include <list>
#include <queue>
#include <unordered_map>
#include <functional>
#include <utility>
#include <SFML/System.hpp>

#include "tilemap.hpp"

// Needed for unordered_map
namespace std
{
    template<>
    struct hash<sf::Vector2u>
    {
        typedef sf::Vector2u argument_type;
        typedef std::size_t result_type;

        result_type operator()(const argument_type& v) const
        {
            const result_type h1 = std::hash<unsigned int>()(v.x);
            const result_type h2 = std::hash<unsigned int>()(v.y);
            return h1 ^ (h2 << 1);
        }
    };
}

// T is the node type
template<typename T>
class Graph
{
public:
    // No edge weights for now
    std::unordered_map<T, std::vector<std::pair<T, float>>> edges;

    Graph() {}

    std::vector<std::pair<T, float>> neighbours(T node)
    {
        return edges[node];
    }
};

// Specialisation for sf::Vector2u
template<>
class Graph<sf::Vector2u>
{
public:
    std::unordered_map<sf::Vector2u, std::vector<std::pair<sf::Vector2u, float>>> edges;

    std::vector<std::pair<sf::Vector2u, float>> neighbours(sf::Vector2u node)
    {
        return edges[node];
    }

    Graph() {}

    // Add nodes at each accessible tile in the tilemap and join
    // them to their neighbours
    Graph<sf::Vector2u>(const Tilemap& tm,
        const std::set<unsigned int>& safe)
    {
        // For every tile in the tilemap, connect each safe tile
        // to all adjacent self tiles by remembering in the set
        // associated with that tile
        for(int y = 0; y < tm.h; ++y)
        {
            for(int x = 0; x < tm.w; ++x)
            {
                // Safe tiles should be joined to adjacent safe tiles
                auto t0 = tm.at(x,y);
                if(safe.count(t0) == 0) continue;
                edges[sf::Vector2u(x,y)] = std::vector<std::pair<sf::Vector2u, float>>();
                for(int dy = -1; dy <= 1; ++dy)
                {
                    if(y+dy < 0 || y+dy >= tm.h) continue;
                    for(int dx = -1; dx <= 1; ++dx)
                    {
                        if(dx == 0 && dy == 0) continue;
                        if(x+dx < 0 || x+dx >= tm.w) continue;
                        auto t1 = tm.at(x+dx, y+dy);
                        if(safe.count(t1) == 0) continue;
                        edges[sf::Vector2u(x, y)].push_back(
                            std::make_pair(sf::Vector2u(x+dx,y+dy), 1.0f));
                    } // Brace cascade of shaaaaame
                }
            }
        }
    }
};

template<typename T>
std::list<T> breadthFirstSearch(Graph<T>* g, const T& start, const T& end)
{
    // Nodes to examine
    std::queue<T> frontier;
    frontier.push(start);
    // Bactrack through this to find the final path
    std::unordered_map<T, T> cameFrom;
    cameFrom[start] = start;

    while(!frontier.empty())
    {
        // Grab next node out of the queue of frontier nodes
        T current = frontier.front();
        frontier.pop();

        for(auto node : g->neighbours(current))
        {
            auto n = node.first; // Ignore edge weights here
            // If n has not come from anywhere, i.e. we haven't
            // visited n yet
            if(cameFrom.count(n) < 1)
            {
                // Add it to the frontier and remember where we
                // came from
                frontier.push(n);
                cameFrom[n] = current;
                // If the neighbouring node was our destination, we
                // are done
                if(n == end)
                {
                    frontier = std::queue<T>();
                    break;
                }
            }
        }
    }
    // Now backtrack from the end node until the start node is reached
    std::list<T> path;
    T current = end;
    while(current != start)
    {
        path.push_front(current);
        current = cameFrom[current];
    }
    return path;
}

#endif /* NAVGRAPH_HPP */
