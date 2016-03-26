#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "tileset.hpp"

class Tilemap : public sf::Drawable, public sf::Transformable
{
private:
    Tileset* mTileset;
    sf::VertexArray mVerts;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    unsigned int w;
    unsigned int h;
    unsigned int ts; // Tilesize

    std::vector<unsigned int> map;

    Tilemap() {}
    Tilemap(const JsonBox::Array& a, Tileset* tileset);

    unsigned int at(unsigned int x, unsigned int y) const;
    unsigned int at(const sf::Vector2f& pos) const;
};

#endif /* TILEMAP_HPP */
