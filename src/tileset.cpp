#include <string>
#include <JsonBox.h>
#include <SFML/Graphics.hpp>

#include "tileset.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

Tileset::Tileset(const std::string& id,
        const std::string& filename,
        unsigned int tilesize) :
    Entity(id),
    tilesize(tilesize)
{
    tex.loadFromFile(filename);
}

Tileset::Tileset(const std::string& id,
        const JsonBox::Value& v,
        EntityManager* mgr) :
    Entity(id)
{
    load(v, mgr);
}

void Tileset::load(const JsonBox::Value& v, EntityManager* mgr)
{
    JsonBox::Object o = v.getObject();
    tilesize = o["tilesize"].getInteger();
    tex.loadFromFile(o["filename"].getString());
    if(o.find("animations") != o.end())
    {
        for(auto a : o["animations"].getArray())
        {
            JsonBox::Object aO = a.getObject();
            animations[aO["name"].getString()] = Animation(
                    aO["x"].getInteger(),
                    aO["y"].getInteger(),
                    aO["len"].getInteger());
        }
    }
}
