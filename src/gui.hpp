#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include "tileset.hpp"

namespace gui
{
    enum Color
    {
        Red = 0,
        Green = 4,
        Blue = 9
    };

class Bar : public sf::Drawable
{
    Tileset* mTileset;
    sf::Sprite mSprite;
    sf::RectangleShape mFill;
    gui::Color mFillCol;
    float mFillRatio;

public:

    Bar() {}
    Bar(Tileset* tileset, gui::Color fillCol, float scale = 1.0f) :
        mTileset(tileset),
        mFillCol(fillCol),
        mFillRatio(1.0f)
    {
        mSprite.setTexture(mTileset->tex);
        mSprite.setTextureRect(sf::IntRect(0, 0, 3 * mTileset->tilesize, mTileset->tilesize));
        mFill.setTexture(&mTileset->tex);
        mFill.setTextureRect(sf::IntRect(3 * mTileset->tilesize + static_cast<int>(mFillCol), 0, 1, 6));
        mFill.setOrigin(sf::Vector2f(-3.0f, -5.0f));
        setFillRatio(mFillRatio);
        mSprite.setScale(scale, scale);
        mFill.setScale(scale, scale);
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(mSprite, states);
        target.draw(mFill, states);
    }
    void setPosition(const sf::Vector2f& pos)
    {
        mSprite.setPosition(pos);
        mFill.setPosition(pos);
    }
    void setFillRatio(float ratio)
    {
        mFillRatio = ratio;
        mFill.setSize(sf::Vector2f(mFillRatio * 3.0f*mTileset->tilesize * 42.0f/48.0f, mTileset->tilesize / 16.0f * 6.0f));
    }
    float getWidth() const
    {
        return mSprite.getGlobalBounds().width;
    }
};
}

#endif /* GUI_HPP */
