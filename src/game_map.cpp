#include <JsonBox.h>

#include "game_map.hpp"
#include "tileset.hpp"
#include "tilemap.hpp"
#include "entity_manager.hpp"
#include "navgraph.hpp"

GameMap::GameMap(const std::string& id, const JsonBox::Value& v,
        EntityManager* mgr) : Entity(id)
{
    load(v, mgr);
}

void GameMap::load(const JsonBox::Value& v, EntityManager* mgr)
{
    JsonBox::Object o = v.getObject();

    if(o.find("tileset") != o.end())
    {
        tileset = mgr->getEntity<Tileset>(o["tileset"].getString());
    }

    if(o.find("tilemap") != o.end())
    {
        JsonBox::Array a = o["tilemap"].getArray();
        tilemap = Tilemap(a, tileset);
        graph = Graph<sf::Vector2u>(tilemap, { 0 });
    }

    if(o.find("spawns") != o.end())
    {
        auto spawns = o["spawns"].getObject();
        for(auto s : spawns["team_1"].getArray())
        {
            auto a = s.getArray();
            team1Spawns.push_back(sf::Vector2f(a[0].getFloat(), a[1].getFloat()));
        }
        for(auto s : spawns["team_2"].getArray())
        {
            auto a = s.getArray();
            team2Spawns.push_back(sf::Vector2f(a[0].getFloat(), a[1].getFloat()));
        }
    }
}
