#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <JsonBox.h>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "entity.hpp"
#include "tileset.hpp"
#include "creature.hpp"

class EntityManager;

class Character : public Creature
{
public:

    Character() : Creature() {}
    Character(const std::string& id, const JsonBox::Value& v, EntityManager* mgr) :
        Creature(id, v, mgr)
    {
        load(v, mgr);
    }

    virtual void load(const JsonBox::Value& v, EntityManager* mgr);

    virtual void update(float dt)
    {
        Creature::update(dt);
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        Creature::draw(target, states);
    }
};

#endif /* CHARACTER_HPP */
