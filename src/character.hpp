#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <JsonBox.h>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "entity.hpp"
#include "tileset.hpp"
#include "creature.hpp"
#include "pathfinding_helper.hpp"

class EntityManager;

class Character : public Creature
{
public:

    PathfindingHelper pfHelper;

    Character() : Creature() {}
    Character(const std::string& id, const JsonBox::Value& v, EntityManager* mgr) :
        Creature(id, v, mgr)
    {
        load(v, mgr);
    }

    virtual void load(const JsonBox::Value& v, EntityManager* mgr);

    virtual void update(float dt)
    {
        // PathfindingHelper update needs a speed, not a time interval,
        // because it doesn't know what it's attached to
        pfHelper.update(dt * getMoveSpeed());
        mPos = pfHelper.pos * (float)mTs;
        Creature::update(dt);
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        Creature::draw(target, states);
    }
};

#endif /* CHARACTER_HPP */
