#include <vector>
#include <SFML/Graphics.hpp>
#include <JsonBox.h>

#include "tileset.hpp"
#include "tilemap.hpp"

Tilemap::Tilemap(const JsonBox::Array& a, Tileset* tileset)
{
    h = a.size();
    if(h > 0)
    {
        // Assume every row has the same length
        w = a[0].getArray().size();
        for(auto row : a)
        {
            for(auto tile : row.getArray())
            {
                // Add tile to map
                map.push_back(tile.getInteger());
            }
        }
    }

    mTileset = tileset;
    ts = tileset->tilesize;

    mVerts.setPrimitiveType(sf::Quads);
    mVerts.resize(4 * w * h);

    for(unsigned int y = 0; y < h; ++y)
    {
        for(unsigned int x = 0; x < w; ++x)
        {
            int u = at(x, y) % (tileset->tex.getSize().x / ts);
            int v = at(x, y) / (tileset->tex.getSize().x / ts);

            sf::Vertex* quad = &mVerts[4 * (y * w + x)];

            // 3---2
            // |   |
            // 0---1
            quad[0].position = sf::Vector2f( x    * ts,  y    * ts);
            quad[1].position = sf::Vector2f((x+1) * ts,  y    * ts);
            quad[2].position = sf::Vector2f((x+1) * ts, (y+1) * ts);
            quad[3].position = sf::Vector2f( x    * ts, (y+1) * ts);
            
            quad[0].texCoords = sf::Vector2f( u    * ts,  v    * ts);
            quad[1].texCoords = sf::Vector2f((u+1) * ts,  v    * ts);
            quad[2].texCoords = sf::Vector2f((u+1) * ts, (v+1) * ts);
            quad[3].texCoords = sf::Vector2f( u    * ts, (v+1) * ts);

            for(int i = 0; i < 4; ++i)
            {
                quad[i].position -= sf::Vector2f(0.5f * ts, 0.5f*ts);
            }
        }
    }
}

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &mTileset->tex;
    target.draw(mVerts, states);
}

unsigned int Tilemap::at(unsigned int x, unsigned int y) const
{
    return map[y * w + x];
}

unsigned int Tilemap::at(const sf::Vector2f& pos) const
{
    return map[(unsigned int)(pos.y * w + pos.x)];
}
