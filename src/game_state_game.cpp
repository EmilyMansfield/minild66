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
    float camPan = ld::cameraPanSpeed * game->map->tilemap.ts * dt;

    // Lambda and constants for shorthand
    auto windowPos = [&window](int x, int y)
    {
        return window.mapPixelToCoords(sf::Vector2i(x, y));
    };
    const unsigned int& w = ld::width;
    const unsigned int& h = ld::height;

    // Camera can only move if it stays within the world bounds
    if((nDir.x < 0 && windowPos(nDir.x*camPan, h/2).x > 0) ||
            (nDir.x > 0 && windowPos(w+nDir.x*camPan, h/2).x < game->map->tilemap.w * game->map->tilemap.ts))
    {
        view.move(nDir.x * camPan, 0.0f);
    }
    if((nDir.y < 0 && windowPos(w/2, nDir.y*camPan).y > 0) ||
            (nDir.y > 0 && windowPos(w/2, h+nDir.y*camPan).y < game->map->tilemap.h * game->map->tilemap.ts))
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
        sf::Vector2f target = window.mapPixelToCoords(sf::Vector2i(
            event.mouseButton.x,
            event.mouseButton.y)) / (float)game->map->tilemap.ts;
        client->c.pfHelper.setTarget(target);
        // Send to server
        NetworkManager::Event e;
        e.type = NetworkManager::Event::Move;
        e.move = {
            .gameId = game->gameId,
            .charId = game->client,
            .target = target,
            .pos = client->c.pfHelper.pos
        };
        nmgr->send(e);
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
    for(const auto& ch : game->characters)
    {
        // Add a health bar is character doesn't have one
        if(characterBars.count(ch.first) == 0)
        {   
            gui::Color barCol;
            if(ch.second.team != client->team) barCol = gui::Color::Red;
            else if(ch.first == game->client) barCol = gui::Color::Green;
            else barCol = gui::Color::Blue;
            characterBars[ch.first] = gui::Bar(mMgr->getEntity<Tileset>("tileset_gui"), barCol, 0.5f);
            characterBars[ch.first].setFillRatio((float)ch.second.c.hp / (float)ch.second.c.hp_max);
        }
        // Set health bar position
        characterBars[ch.first].setPosition(ch.second.c.getPos() - sf::Vector2f(
            characterBars[ch.first].getWidth() / 2.0f,
            game->map->tileset->tilesize));
    }
    game->update(dt);
}

