#ifndef TARGET_ATTACK_HPP
#define TARGET_ATTACK_HPP

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <functional>
#include <string>

#include "tileset.hpp"
#include "game_container.hpp"
#include "network_manager.hpp"

// When something attacks, this class is created and given an animation
// and action to execute. update should be called each frame, then when
// the animation reaches a specified trigger frame, the action is
// executed, applying the attack. The function should accept two
// CharWrappers corresponding to the source and target, then create and
// return a NetEvent which encapsulates the attack
class TargetAttack
{
private:

    sf::Uint8 mSource;
    sf::Uint8 mTarget;

    Tileset* mTileset;
    unsigned int mTs;
    Animation* mAnim;
    float mAnimT;
    unsigned int mAnimCurrentFrame;
    const unsigned int mTriggerFrame;
    bool mIsDone;

    std::function<NetworkManager::Event(
        const GameContainer::CharWrapper&,
        const GameContainer::CharWrapper&)> mFunc;
    GameContainer* mGame;

    sf::Sprite mSprite;

    NetworkManager::Event mNetEvent;

public:

    TargetAttack(sf::Uint8 source, sf::Uint8 target, Tileset* tileset,
        const std::string& animation, unsigned int triggerFrame,
        std::function<NetworkManager::Event(
            const GameContainer::CharWrapper&,
            const GameContainer::CharWrapper&)> f,
        GameContainer* game) :
        mSource(source),
        mTarget(target),
        mTileset(tileset),
        mTs(tileset->tilesize),
        mAnimT(0.0f),
        mAnimCurrentFrame(0),
        mTriggerFrame(triggerFrame),
        mIsDone(false),
        mFunc(f),
        mGame(game)
    {
        mSprite.setTexture(mTileset->tex);
        mAnim = &mTileset->animations[animation];
        mSprite.setTextureRect(sf::IntRect(mAnim->x, mAnim->y, mTs, mTs));
        mSprite.setOrigin(mTs/2.0, mTs/2.0);
    }

    bool update(float dt)
    {
        mAnimT += dt;
        int frame = mAnimT * mAnim->len / mAnim->duration;
        if(frame >= mAnim->len)
        {
            mIsDone = true;
            frame %= mAnim->len;
        }
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
        if(frame == mTriggerFrame)
        {
            // TODO: Trigger also if trigger frame is skipped
            mNetEvent = mFunc(mGame->characters[mSource], mGame->characters[mTarget]);
        }
        return mIsDone;
    }

    void setPos(const sf::Vector2f& pos) { mSprite.setPosition(pos); }
    bool isDone() const { return mIsDone; }
    NetworkManager::Event getEvent() const { return mNetEvent; }
};

#endif /* TARGET_ATTACK_HPP */
