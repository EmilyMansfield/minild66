#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "game_state.hpp"
#include "entity_manager.hpp"
#include "game_state_game.hpp"

class GameStateTitle : public GameState
{
public:
    GameStateTitle(std::shared_ptr<GameState>& state,
            std::shared_ptr<GameState>& prevState,
            EntityManager* mgr) :
        GameState(state, prevState, mgr)
    {}

    virtual void handleEvent(const sf::Event& event,
            const sf::RenderWindow& window);
    virtual void handleInput(float dt,
            const sf::RenderWindow& window);
    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
    }
};

#endif /* GAME_STATE_TITLE_HPP */
