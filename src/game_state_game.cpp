#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "game_state_game.hpp"
#include "constants.hpp"

void GameStateGame::handleEvent(const sf::Event& event)
{
}

void GameStateGame::handleInput(float dt)
{
    float camPan = ld::cameraPanSpeed * dt * map->tilemap.ts * zoomLevel;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        view.move(-camPan, 0.0f);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        view.move(camPan, 0.0f);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        view.move(0.0f, -camPan);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        view.move(0.0f, camPan);
    }
}

void GameStateGame::update(float dt)
{
}

