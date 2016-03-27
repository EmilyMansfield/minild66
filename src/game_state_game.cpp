#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "game_state_game.hpp"
#include "constants.hpp"
#include "vecmath.hpp"

void GameStateGame::pan(const sf::Vector2f& dir, float dt, const sf::RenderWindow& window)
{
    // Normalise direction vector
    const sf::Vector2f nDir = dir / vecmath::norm<float>(dir);
    // Camera panning speed
    float camPan = ld::cameraPanSpeed * map->tilemap.ts * zoomLevel * dt;
    // Lambda and constants for shorthand
    auto windowPos = [&window](int x, int y)
    {
        return window.mapPixelToCoords(sf::Vector2i(x, y));
    };
    const unsigned int& w = ld::width;
    const unsigned int& h = ld::height;
    // Camera can only move if it stays within the world bounds
    if((nDir.x < 0 && windowPos(nDir.x*camPan, h/2).x > 0) ||
            (nDir.x > 0 && windowPos(w+nDir.x*camPan, h/2).x < map->tilemap.w * map->tilemap.ts))
    {
        view.move(nDir.x * camPan, 0.0f);
    }
    if((nDir.y < 0 && windowPos(w/2, nDir.y*camPan).y > 0) ||
            (nDir.y > 0 && windowPos(w/2, h+nDir.y*camPan).y < map->tilemap.h * map->tilemap.ts))
    {
        view.move(0.0f, nDir.y * camPan);
    }
}

void GameStateGame::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
}

void GameStateGame::handleInput(float dt, const sf::RenderWindow& window)
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        pan(sf::Vector2f(-1.0f, 0.0f), dt, window);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        pan(sf::Vector2f(1.0f, 0.0f), dt, window);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        pan(sf::Vector2f(0.0f, -1.0f), dt, window);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        pan(sf::Vector2f(0.0f, 1.0f), dt, window);
    }
}

void GameStateGame::update(float dt)
{
}

