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
#include "game_container.hpp"
#include "network_manager.hpp"
#include "gui.hpp"

class GameStateGame : public GameState
{
private:
    sf::View view;
    std::shared_ptr<GameContainer> game;
    GameContainer::CharWrapper* client;
    sf::Vector2f* pathfindPtr;
    NetworkManager* nmgr;
    std::map<sf::Uint8, gui::Bar> characterBars;

    void pan(const sf::Vector2f& dir, float dt, const sf::RenderWindow& window);

public:
    GameStateGame(std::shared_ptr<GameState>& state,
            std::shared_ptr<GameState>& prevState,
            std::shared_ptr<GameContainer> game,
            EntityManager* mgr,
            NetworkManager* nmgr) :
        GameState(state, prevState, mgr),
        game(game),
        pathfindPtr(nullptr),
        nmgr(nmgr)
    {
        view = sf::View(sf::FloatRect(0, 0,
            ld::widthTiles * game->map->tilemap.ts,
            ld::heightTiles * game->map->tilemap.ts));

        // Get a pointer to the client's character
        client = game->getClient();

        // Centre the view on the client
        view.setCenter((float)game->map->tilemap.ts * client->c.pfHelper.pos);
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
        target.draw(game->map->tilemap, states);
        for(const auto& ch : game->characters)
        {
            target.draw(ch.second.c, states);
            target.draw(characterBars.at(ch.first), states);
        }
    }
};

#endif /* GAME_STATE_GAME_HPP */
