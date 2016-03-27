#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <JsonBox.h>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "entity.hpp"
#include "tileset.hpp"

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

class Creature : public Entity, public sf::Drawable
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

    // TODO: Add these to a component instead of just hacking them in?
    sf::Vector2f mPos;
    sf::Vector2f mVel;
    sf::Sprite mSprite;
    Tileset* mTileset;
    unsigned int mTs;
    Animation* mAnim;
    float mAnimT;
    unsigned int mAnimCurrentFrame;

public:

    Creature() : Entity("nullid") {}
    Creature(const std::string& id, const JsonBox::Value& v, EntityManager* mgr) :
        Entity(id)
    {
        load(v, mgr);
    }

    virtual void load(const JsonBox::Value& v, EntityManager* mgr);

    virtual void update(float dt)
    {
        mSprite.setPosition(mPos);
        int frame = mAnimT * mAnim->duration / mAnim->len;
        // No point changing the frame when it doesn't need to be
        if(frame != mAnimCurrentFrame)
        {
            mSprite.setTextureRect(sf::IntRect(
                    mAnim->x + frame * mTs,
                    mAnim->y,
                    mTs,
                    mTs));
            mAnimCurrentFrame = frame;
        }
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(mSprite);
    }
};

#endif /* CREATURE_HPP */
