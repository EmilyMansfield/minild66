#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <SFML/System.hpp>

#include "tileset.hpp"
#include "tilemap.hpp"
#include "navgraph.hpp"

class GameMap : public Entity
{
public:

    Tileset* tileset;
    Tilemap tilemap;
    Graph<sf::Vector2u> graph;

    GameMap(const std::string& id, const JsonBox::Value& v, EntityManager* mgr);

    void load(const JsonBox::Value& v, EntityManager* mgr);
};

#endif /* GAME_MAP_HPP */
