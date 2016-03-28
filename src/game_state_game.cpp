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
    // Moving / cancelling current action
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        // Set the pathfinding target
        playerCharacter.pfHelper.setTarget(
            window.mapPixelToCoords(sf::Vector2i(
                event.mouseButton.x,
                event.mouseButton.y)) / (float)map->tilemap.ts);
    }
}

void GameStateGame::handleInput(float dt, const sf::RenderWindow& window)
{
    // Panning using the keyboard
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

    // Panning using the mouse
    // Direction vectors cannot be taken directly, they must be scaled
    // to line within [-w/2, w/2] on both axes
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    const float w = ld::width;
    const float h = ld::height;
    if(mousePos.x < 2)
    {
        pan(sf::Vector2f(-w/2.0f, w*((float)mousePos.y/h - 0.5f)), dt, window);
    }
    else if(mousePos.x > ld::width - 2)
    {
        pan(sf::Vector2f(w/2.0f, w*((float)mousePos.y/h - 0.5f)), dt, window);
    }
    else if(mousePos.y < 2)
    {
        pan(sf::Vector2f((float)mousePos.x - w/2.0f, -w/2.0f), dt, window);
    }
    else if(mousePos.y > ld::height - 2)
    {
        pan(sf::Vector2f((float)mousePos.x - w/2.0f, w/2.0f), dt, window);
    }
}

void GameStateGame::update(float dt)
{
    playerCharacter.update(dt);
}

