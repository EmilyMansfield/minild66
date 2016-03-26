#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include "tileset.hpp"
#include "tilemap.hpp"

class GameMap : public Entity
{
public:

    Tileset* tileset;
    Tilemap tilemap;

    GameMap(const std::string& id, const JsonBox::Value& v, EntityManager* mgr);

    void load(const JsonBox::Value& v, EntityManager* mgr);
};

#endif /* GAME_MAP_HPP */
