#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <memory>

// I should really just make a library with this definition,
// almost everything I write seems to use it
class GameState : public sf::Drawable
{
protected:
    std::shared_ptr<GameState>& mState;
    std::shared_ptr<GameState>& mPrevState;

public:
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void handleInput(float dt) = 0;
    virtual void update(float dt) = 0;

    GameState(std::shared_ptr<GameState>& state,
            std::shared_ptr<GameState>& prevState) :
        mState(state),
        mPrevState(prevState)
    {
    }

    virtual ~GameState() {}
};

#endif /* GAME_STATE_HPP */
