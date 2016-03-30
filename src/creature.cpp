#include <JsonBox.h>
#include "entity_manager.hpp"
#include "creature.hpp"

void Creature::load(const JsonBox::Value& v, EntityManager* mgr)
{
    JsonBox::Object o = v.getObject();

    auto has = [&o](const std::string& s) { return o.find(s) != o.end(); };

    // Load main properties
    if(has("hp_max")) hp_max = o["hp_max"].tryGetFloat(0.0f);
    hp = has("hp") ? o["hp"].tryGetFloat(hp_max) : hp_max;
    if(has("hp_regen")) hp_regen = o["hp_regen"].tryGetFloat(0.0f);
    if(has("mp_max")) mp_max = o["mp_max"].tryGetFloat(0.0f);
    mp = has("mp") ? o["mp"].tryGetFloat(mp_max) : mp_max;
    if(has("mp_regen")) mp_regen = o["mp_regen"].tryGetFloat(0.0f);
    if(has("moveSpeed")) moveSpeed = o["moveSpeed"].tryGetFloat(0.0f);
    if(has("pd")) pd = o["pd"].tryGetFloat(0.0f);
    if(has("sd")) sd = o["sd"].tryGetFloat(0.0f);
    if(has("pa")) pa = o["pa"].tryGetFloat(0.0f);
    if(has("sa")) sa = o["sa"].tryGetFloat(0.0f);

    // Load stats
    if(has("stats"))
    {
        JsonBox::Object statsO = o["stats"].getObject();
        if(has("str")) stats.str = statsO["str"].tryGetFloat(0.0f);
        if(has("dex")) stats.dex = statsO["dex"].tryGetFloat(0.0f);
        if(has("con")) stats.con = statsO["con"].tryGetFloat(0.0f);
        if(has("mnd")) stats.mnd = statsO["mnd"].tryGetFloat(0.0f);
        if(has("wis")) stats.wis = statsO["wis"].tryGetFloat(0.0f);
        if(has("lck")) stats.lck = statsO["lck"].tryGetFloat(0.0f);
    }

    // Load tileset
    if(has("tileset"))
    {
        mTileset = mgr->getEntity<Tileset>(o["tileset"].tryGetString("nullid"));
        mTs = mTileset->tilesize;
        mSprite.setTexture(mTileset->tex);
        mAnim = &mTileset->animations[id + "_idle"];
        mSprite.setTextureRect(sf::IntRect(mAnim->x, mAnim->y, mTs, mTs));
        mAnimT = 0.0f;
        mAnimCurrentFrame = 0;
        mSprite.setOrigin(mTs/2.0, mTs/2.0);
    }
}
