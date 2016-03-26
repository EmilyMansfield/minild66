#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "game_state.hpp"

class GameStateTitle : public GameState
{
public:
    GameStateTitle(std::shared_ptr<GameState>& state,
            std::shared_ptr<GameState>& prevState) :
        GameState(state, prevState)
    {
    }

    virtual void handleEvent(const sf::Event& event);
    virtual void handleInput(float dt);
    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
    }
};

#endif /* GAME_STATE_TITLE_HPP */
