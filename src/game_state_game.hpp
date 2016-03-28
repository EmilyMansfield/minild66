#ifndef GAME_STATE_GAME_HPP
#define GAME_STATE_GAME_HPP

#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "game_state.hpp"
#include "entity_manager.hpp"
#include "game_map.hpp"
#include "constants.hpp"
#include "character.hpp"
#include "pathfinding_helper.hpp"

class GameStateGame : public GameState
{
private:
    GameMap* map;
    sf::View view;
    float zoomLevel;
    Character playerCharacter;

    void pan(const sf::Vector2f& dir, float dt, const sf::RenderWindow& window);

public:
    GameStateGame(std::shared_ptr<GameState>& state,
            std::shared_ptr<GameState>& prevState,
            EntityManager* mgr) :
        GameState(state, prevState, mgr)
    {
        map = mgr->getEntity<GameMap>("gamemap_5v5");
        zoomLevel = 2.0f;
        view = sf::View(sf::FloatRect(0, 0, ld::width / zoomLevel, ld::height / zoomLevel));
        view.setCenter(sf::Vector2f(
                    map->tilemap.ts * map->tilemap.w / 2.0f,
                    map->tilemap.ts * map->tilemap.h / 2.0f));

        sf::Vector2f startPos(map->tilemap.w / 4.0f, map->tilemap.h / 4.0f);
        playerCharacter = *mgr->getEntity<Character>("character_fighter");
        playerCharacter.pfHelper = PathfindingHelper(startPos, startPos, &map->graph);
    }

    virtual void handleEvent(const sf::Event& event,
            const sf::RenderWindow& window);
    virtual void handleInput(float dt,
            const sf::RenderWindow& window);
    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.clear(sf::Color::Green);
        target.setView(view);
        target.draw(map->tilemap, states);
        target.draw(playerCharacter);
    }
};

#endif /* GAME_STATE_GAME_HPP */
