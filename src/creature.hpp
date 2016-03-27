#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <JsonBox.h>
#include <string>

#include "entity.hpp"

class EntityManager;

typedef struct
{
    // Strength: Physical damage
    float str;
    // Dexterity: Movement speed, attack speed
    float dex;
    // Constitution: Max HP, HP regen, physical armour
    float con;
    // Mind: spell damage, mana regen
    float mnd;
    // Wisdom: Max Mana, spell armour
    float wis;
    // Luck: Crit rate, rng bias
    float lck;
} Stats;

class Creature : public Entity
{
protected:
    // Health
    float hp;
    float hp_max;
    float hp_regen; // Per second
    // Mana
    float mp;
    float mp_max;
    float mp_regen;
    // Movement speed (tiles per second)
    float moveSpeed;
    // Strength, Constitution, etc.
    Stats stats;
    // Physical and spell damage, calculated from str etc.
    float pd;
    float sd;
    // Physical and spell armour, calculated from con etc.
    float pa;
    float sa;

public:

    Creature() : Entity("nullid") {}
    Creature(const std::string& id, const JsonBox::Value& v, EntityManager* mgr) :
        Entity(id)
    {
        load(v, mgr);
    }

    virtual void load(const JsonBox::Value& v, EntityManager* mgr);
};

#endif /* CREATURE_HPP */
