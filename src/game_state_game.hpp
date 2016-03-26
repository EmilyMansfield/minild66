#ifndef GAME_STATE_GAME_HPP
#define GAME_STATE_GAME_HPP

#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "game_state.hpp"
#include "entity_manager.hpp"
#include "game_map.hpp"
#include "constants.hpp"

class GameStateGame : public GameState
{
private:
    GameMap* map;
    sf::View view;

public:
    GameStateGame(std::shared_ptr<GameState>& state,
            std::shared_ptr<GameState>& prevState,
            EntityManager* mgr) :
        GameState(state, prevState, mgr)
    {
        map = mgr->getEntity<GameMap>("gamemap_5v5");
        view = sf::View(sf::FloatRect(0, 0, ld::width, ld::height));
        view.setCenter(sf::Vector2f(
                    map->tilemap.ts * map->tilemap.w / 2.0f,
                    map->tilemap.ts * map->tilemap.h / 2.0f));
    }

    virtual void handleEvent(const sf::Event& event);
    virtual void handleInput(float dt);
    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.clear(sf::Color::Green);
        target.setView(view);
        target.draw(map->tilemap, states);
    }
};

#endif /* GAME_STATE_GAME_HPP */
