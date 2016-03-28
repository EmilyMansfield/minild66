#ifndef NAVGRAPH_HPP
#define NAVGRAPH_HPP

#include <set>
#include <vector>
#include <SFML/System.hpp>

#include "graph.hpp"
#include "tilemap.hpp"

class Navgraph
{
public:

    std::vector<std::set<sf::Vector2u>> graph;

    Navgraph() {}

    void create(const Tilemap& tm, const std::set<unsigned int>& safe)
    {
        // Populate the graph with isolated empty nodes
        graph = std::vector<std::set<sf::Vector2f>>(
                tm.w*tm.h,
                std::set<std::Vector2u>());
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
                for(int dy = -1; dy <= 1; ++dy)
                {
                    if(y+dy < 0 || y+dy >= tm.h) continue;
                    for(int dx = -1; dx <= 1; ++dx)
                    {
                        if(dx == 0 && dy == 0) continue;
                        if(x+dx < 0 || x+dx >= tm.w) continue;
                        auto t1 = tm.at(x+dx, y+dy);
                        if(safe.count(t1) == 0) continue;
                        graph[y*tm.w+x].insert(sf::Vector2u(x+dx,y+dy));
                    } // Brace cascade of shaaaaame
                }
            }
        }
    }

    
};

#endif /* NAVGRAPH_HPP */
