#ifndef TILESET_HPP
#define TILESET_HPP

#include <string>
#include <JsonBox.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "entity.hpp"

class EntityManager;

class Animation
{
public:
    unsigned int x;
    unsigned int y;
    unsigned int len;
    float duration;

    Animation() {}
    Animation(unsigned int pX, unsigned int pY, unsigned int pLen, float pDuration) :
        x(pX), y(pY), len(pLen), duration(pDuration) {}
};

class Tileset : public Entity
{
public:
    sf::Texture tex;
    unsigned int tilesize;

    std::map<std::string, Animation> animations;

    Tileset(const std::string& id,
            const std::string& filename,
            unsigned int tilesize);
    Tileset(const std::string& id,
            const JsonBox::Value& v,
            EntityManager* mgr);

    virtual void load(const JsonBox::Value& v, EntityManager* mgr);
};

#endif /* TILESET_HPP */
